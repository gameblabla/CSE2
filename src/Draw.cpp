#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOL BOOL_OGC
#include <gccore.h>
#undef BOOL

#include "EasyBMP/EasyBMP_DataStructures.h"
#include "EasyBMP/EasyBMP_BMP.h"
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Input.h"
#include "Resource.h"
#include "Tags.h"
#include "Types.h"

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

SURFACE surf[SURFACE_ID_MAX];

//Some stuff to convert RGB to YUV because the Wii is horrible!!! :D
BUFFER_PIXEL RGBToYUV(uint8_t red, uint8_t green, uint8_t blue)
{
	BUFFER_PIXEL yuv;
	/*
	yuv.y = (299 * red + 587 * green + 114 * blue) / 1000;
	yuv.u = (-16874 * red - 33126 * green + 50000 * blue + 12800000) / 100000;
	yuv.v = (50000 * red - 41869 * green - 8131 * blue + 12800000) / 100000;
	*/
	yuv.y = 16  + (((red << 6) + (red << 1) + (green << 7) + green + (blue << 4) + (blue << 3) + blue) >> 8);
	yuv.u = 128 + ((-((red << 5) + (red << 2) + (red << 1)) - ((green << 6) + (green << 3) + (green << 1)) + (blue << 7) - (blue << 4)) >> 8);
	yuv.v = 128 + (((red << 7) - (red << 4) - ((green << 6) + (green << 5) - (green << 1)) - ((blue << 4) + (blue << 1))) >> 8);
	return yuv;
}

//Frame-buffers and screen mode
#pragma pack(push)
#pragma pack(1)
struct WII_SOFT_BUFFER
{
	uint8_t y1, cb, y2, cr;
};
#pragma pack(pop)

uint32_t *xfb[2];
WII_SOFT_BUFFER *screenBuffer;

bool currentFramebuffer = false;
GXRModeObj *prefMode;

#define SET_SOFT_PIXEL(tx, ty, pixel) \
	WII_SOFT_BUFFER *pix = &screenBuffer[(tx >> 1) + ty * WINDOW_WIDTH];							\
	WII_SOFT_BUFFER *dup = &screenBuffer[((tx >> 1) + ty * WINDOW_WIDTH) + (WINDOW_WIDTH >> 1)];	\
	if (tx & 0x1)																					\
	{																								\
		pix->y2 = dup->y2 = pixel.y;																\
		pix->cb = dup->cb = (pix->cb + pixel.u) >> 1;												\
		pix->cr = dup->cr = (pix->cr + pixel.v) >> 1;												\
	}																								\
	else																							\
	{																								\
		pix->y1 = dup->y1 = pixel.y;																\
		pix->cb = dup->cb = pixel.u;																\
		pix->cr = dup->cr = pixel.v;																\
	}
	

#define SET_BUFFER_PIXEL(buffer, w, tx, ty, sy, su, sv)	\
	buffer[ty * w + tx].y = sy;							\
    buffer[ty * w + tx].u = su;							\
    buffer[ty * w + tx].v = sv;
	
//Draw to screen
BOOL Flip_SystemTask()
{
	//Update inputs
	UpdateInput();
	
	//Write to framebuffer
	memcpy(xfb[currentFramebuffer], screenBuffer, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(WII_SOFT_BUFFER));
	
	//Flush screen to our television and wait for next frame
	VIDEO_SetNextFramebuffer(xfb[currentFramebuffer]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	
	//Swap frame-buffer
	currentFramebuffer = !currentFramebuffer;
	return TRUE;
}

//Initialize video and frame-buffers
BOOL StartDirectDraw()
{
	//Initialize video subsystem
	VIDEO_Init();
	
	//Set our preferred settings
	prefMode = VIDEO_GetPreferredMode(NULL);
	prefMode->fbWidth = WINDOW_WIDTH;
	prefMode->viWidth = 720;
	prefMode->viXOrigin = 0;
	
	//Allocate frame-buffer memory
	xfb[0] = (uint32_t*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(prefMode));
	xfb[1] = (uint32_t*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(prefMode));
	
	screenBuffer = (WII_SOFT_BUFFER*)malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(WII_SOFT_BUFFER));
	
	//Setup the video registers with the chosen mode
	VIDEO_Configure(prefMode);
	
	//Tell the VDP where are frame-buffer is
	VIDEO_SetNextFramebuffer(xfb[0]);
	
	//Make the display visible
	VIDEO_SetBlack(FALSE);
	
	//Flush to the VDP
	VIDEO_Flush();

	//Wait for video to be fully setup
	VIDEO_WaitVSync();
	
	if (prefMode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
	return TRUE;
}

void EndDirectDraw()
{
	free(screenBuffer);
}

void ReleaseSurface(int s)
{
	free(surf[s].data);
}

BOOL MakeSurface_Generic(int bxsize, int bysize, Surface_Ids surf_no)
{
	if (surf_no < SURFACE_ID_MAX)
	{
		free(surf[surf_no].data);
		surf[surf_no].w = bxsize;
		surf[surf_no].h = bysize;
		surf[surf_no].data = (BUFFER_PIXEL*)malloc(bxsize * bysize * sizeof(BUFFER_PIXEL));
	}

	return TRUE;
}

BOOL LoadBitmap(BMP *bmp, Surface_Ids surf_no, bool create_surface)
{
	if (create_surface)
		MakeSurface_Generic(bmp->TellWidth(), bmp->TellHeight(), surf_no);
	else
		memset(surf[surf_no].data, 0, surf[surf_no].w * surf[surf_no].h * sizeof(BUFFER_PIXEL));
	
	for (int x = 0; x < bmp->TellWidth(); x++)
	{
		for (int y = 0; y < bmp->TellHeight(); y++)
		{
			if (x >= surf[surf_no].w || y >= surf[surf_no].h)
				continue;
			RGBApixel pixel = bmp->GetPixel(x, y);
			surf[surf_no].data[y * surf[surf_no].w + x] = RGBToYUV(pixel.Red, pixel.Green, pixel.Blue);
		}
	}
	
	return TRUE;
}

BOOL LoadBitmap_File(const char *name, Surface_Ids surf_no, bool create_surface)
{
	BMP *bmp = new BMP;
	
	if (bmp)
	{
		char pbmPath[PATH_LENGTH];
		char bmpPath[PATH_LENGTH];
		sprintf(pbmPath, "%s/%s.pbm", gDataPath, name);
		sprintf(bmpPath, "%s/%s.bmp", gDataPath, name);
		
		if (bmp->ReadFromFile(pbmPath) || bmp->ReadFromFile(bmpPath))
		{
			LoadBitmap(bmp, surf_no, create_surface);
			delete bmp;
			return TRUE;
		}
		
		delete bmp;
	}
	
	return FALSE;
}

BOOL LoadBitmap_Resource(const char *res, Surface_Ids surf_no, bool create_surface)
{
	size_t resSize;
	const unsigned char *resd = FindResource(res, &resSize);
	
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s.bmp", gDataPath, res);
	
	FILE *temp = fopen(path, "wb");
	if (!temp)
		return FALSE;
	
	fwrite(resd, resSize, 1, temp);
	fclose(temp);
	
	LoadBitmap_File(res, surf_no, create_surface);
	
	remove(path);
	return TRUE;
}

BOOL MakeSurface_File(const char *name, Surface_Ids surf_no)
{
	return LoadBitmap_File(name, surf_no, true);
}

BOOL MakeSurface_Resource(const char *res, Surface_Ids surf_no)
{
	return LoadBitmap_Resource(res, surf_no, true);
}

BOOL ReloadBitmap_File(const char *name, Surface_Ids surf_no)
{
	return LoadBitmap_File(name, surf_no, false);
}

BOOL ReloadBitmap_Resource(const char *res, Surface_Ids surf_no)
{
	return LoadBitmap_Resource(res, surf_no, false);
}

void BackupSurface(Surface_Ids surf_no, RECT *rect)
{
	for (int fx = rect->left; fx < rect->right; fx++)
	{
		for (int fy = rect->top; fy < rect->bottom; fy++)
		{
			int dx = fx - rect->left;
			int dy = fy - rect->top;
			
			if (dx < 0 || dy < 0)
				continue;
			if (dx >= surf[surf_no].w || dy >= surf[surf_no].h)
				continue;
			
			WII_SOFT_BUFFER *fromPixel = &screenBuffer[fy * WINDOW_WIDTH + (fx >> 1)];
			uint8_t y;
			if (fx & 0x1)
				y = fromPixel->y2;
			else
				y = fromPixel->y1;
			SET_BUFFER_PIXEL(surf[surf_no].data, surf[surf_no].w, dx, dy, y, fromPixel->cb, fromPixel->cr);
		}
	}
}

static void DrawBitmap(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no, bool transparent)
{
	if (surf[surf_no].data)
	{
		//Clip our rect
		RECT renderRect;
		renderRect.left = (x < rcView->left) ? (rect->left + (rcView->left - x)) : rect->left;
		renderRect.top = (y < rcView->top) ? (rect->top + (rcView->top - y)) : rect->top;
		renderRect.right = ((x + rect->right - rect->left) >= rcView->right) ? rect->right - ((x + rect->right - rect->left) - rcView->right) : rect->right;
		renderRect.bottom = ((y + rect->bottom - rect->top) >= rcView->bottom) ? rect->bottom - ((y + rect->bottom - rect->top) - rcView->bottom) : rect->bottom;
		
		//Clip our draw position
		for (int fx = renderRect.left; fx < renderRect.right; fx++)
		{
			for (int fy = renderRect.top; fy < renderRect.bottom; fy++)
			{
				int dx = x + (fx - rect->left);
				int dy = y + (fy - rect->top);
				
				BUFFER_PIXEL pixel = surf[surf_no].data[fy * surf[surf_no].w + fx];
				if (transparent && pixel.y <= 16)
					continue;
				SET_SOFT_PIXEL(dx, dy, pixel);
			}
		}
	}
}

void PutBitmap3(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no) //Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, true);
}

void PutBitmap4(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no) //No Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, false);
}

void Surface2Surface(int x, int y, RECT *rect, int to, int from)
{
	if (surf[from].data && surf[to].data)
	{
		//Clip our rect
		RECT renderRect;
		renderRect.left = (x < 0) ? (rect->left + (0 - x)) : rect->left;
		renderRect.top = (y < 0) ? (rect->top + (0 - y)) : rect->top;
		renderRect.right = ((x + rect->right - rect->left) >= surf[to].w) ? rect->right - ((x + rect->right - rect->left) - surf[to].w) : rect->right;
		renderRect.bottom = ((y + rect->bottom - rect->top) >= surf[to].h) ? rect->bottom - ((y + rect->bottom - rect->top) - surf[to].h) : rect->bottom;
		
		for (int fx = renderRect.left; fx < renderRect.right; fx++)
		{
			for (int fy = renderRect.top; fy < renderRect.bottom; fy++)
			{
				int dx = x + (fx - rect->left);
				int dy = y + (fy - rect->top);
				
				BUFFER_PIXEL pixel = surf[from].data[fy * surf[from].w + fx];
				if (pixel.y <= 16) //Surface2Surface is always color keyed
					continue;
				SET_BUFFER_PIXEL(surf[to].data, surf[to].w, dx, dy, pixel.y, pixel.u, pixel.v);
			}
		}
	}
}

unsigned long GetCortBoxColor(unsigned long col)
{
	// This comes in BGR, and goes out BGR
	return col;
}

void CortBox(RECT *rect, uint32_t col)
{
	const unsigned char col_red = col & 0x0000FF;
	const unsigned char col_green = (col & 0x00FF00) >> 8;
	const unsigned char col_blue = (col & 0xFF0000) >> 16;
	const BUFFER_PIXEL colPixel = RGBToYUV(col_red, col_green, col_blue);
	
	for (int y = (rect->top < 0 ? 0 : rect->top); y < (rect->bottom >= WINDOW_HEIGHT ? WINDOW_HEIGHT : rect->bottom); y++)
	{
		for (int x = (rect->left < 0 ? 0 : rect->left); x < (rect->right >= WINDOW_WIDTH ? WINDOW_WIDTH : rect->right); x++)
		{
			SET_SOFT_PIXEL(x, y, colPixel);
		}
	}
}

void CortBox2(RECT *rect, uint32_t col, Surface_Ids surf_no)
{
	if (surf[surf_no].data)
	{
		const unsigned char col_red = col & 0x0000FF;
		const unsigned char col_green = (col & 0x00FF00) >> 8;
		const unsigned char col_blue = (col & 0xFF0000) >> 16;
		const BUFFER_PIXEL colPixel = (BUFFER_PIXEL)RGBToYUV(col_red, col_green, col_blue);
		
		for (int y = (rect->top < 0 ? 0 : rect->top); y < (rect->bottom >= surf[surf_no].h ? surf[surf_no].h : rect->bottom); y++)
		{
			for (int x = (rect->left < 0 ? 0 : rect->left); x < (rect->right >= surf[surf_no].w ? surf[surf_no].w : rect->right); x++)
			{
				SET_BUFFER_PIXEL(surf[surf_no].data, surf[surf_no].w, x, y, colPixel.y, colPixel.u, colPixel.v);
			}
		}
	}
}

void InitTextObject()
{
	MakeSurface_File("Font", SURFACE_ID_FONT);
}

void PutText(int x, int y, const char *text, uint32_t color)
{
	RECT rcCharacter;
	RECT *rcView = &grcFull;
	RECT *rect = &rcCharacter;
	RECT renderRect;
	
	//Get our surface colour
	int r = (color & 0xFF0000) >> 16;
	int g = (color & 0x00FF00) >>  8;
	int b = (color & 0x0000FF) >>  0;
	const BUFFER_PIXEL colPixel = (BUFFER_PIXEL)RGBToYUV(r, g, b);
	
	for (int i = 0; i < strlen(text); i++)
	{
		rcCharacter.left = (text[i] % 0x20) * 12;
		rcCharacter.top = (text[i] / 0x20 - 1) * 12;
		rcCharacter.right = rcCharacter.left + 12;
		rcCharacter.bottom = rcCharacter.top + 12;
		
		if (surf[SURFACE_ID_FONT].data)
		{
			//Clip our rect
			renderRect.left = (x < rcView->left) ? (rect->left + (rcView->left - x)) : rect->left;
			renderRect.top = (y < rcView->top) ? (rect->top + (rcView->top - y)) : rect->top;
			renderRect.right = ((x + rect->right - rect->left) >= rcView->right) ? rect->right - ((x + rect->right - rect->left) - rcView->right) : rect->right;
			renderRect.bottom = ((y + rect->bottom - rect->top) >= rcView->bottom) ? rect->bottom - ((y + rect->bottom - rect->top) - rcView->bottom) : rect->bottom;
			
			for (int fx = renderRect.left; fx < renderRect.right; fx++)
			{
				for (int fy = renderRect.top; fy < renderRect.bottom; fy++)
				{
					int dx = (x + 5 * i) + (fx - rect->left);
					int dy = y + (fy - rect->top);
					
					BUFFER_PIXEL pixel = surf[SURFACE_ID_FONT].data[fy * surf[SURFACE_ID_FONT].w + fx];
					if (pixel.y <= 16)
						continue;
					SET_SOFT_PIXEL(dx, dy, colPixel);
				}
			}
		}
	}
}

void PutText2(int x, int y, const char *text, uint32_t color, Surface_Ids surf_no)
{
	RECT rcCharacter;
	RECT *rcView = &grcFull;
	RECT *rect = &rcCharacter;
	RECT renderRect;
	
	//Get our surface colour
	int r = (color & 0xFF0000) >> 16;
	int g = (color & 0x00FF00) >>  8;
	int b = (color & 0x0000FF) >>  0;
	const BUFFER_PIXEL colPixel = (BUFFER_PIXEL)RGBToYUV(r, g, b);
	
	for (int i = 0; i < strlen(text); i++)
	{
		rcCharacter.left = (text[i] % 0x20) * 12;
		rcCharacter.top = (text[i] / 0x20 - 1) * 12;
		rcCharacter.right = rcCharacter.left + 12;
		rcCharacter.bottom = rcCharacter.top + 12;
		
		if (surf[SURFACE_ID_FONT].data && surf[surf_no].data)
		{
			//Clip our rect
			renderRect.left = (x < 0) ? (rect->left + (0 - x)) : rect->left;
			renderRect.top = (y < 0) ? (rect->top + (0 - y)) : rect->top;
			renderRect.right = ((x + rect->right - rect->left) >= surf[surf_no].w) ? rect->right - ((x + rect->right - rect->left) - surf[surf_no].w) : rect->right;
			renderRect.bottom = ((y + rect->bottom - rect->top) >= surf[surf_no].h) ? rect->bottom - ((y + rect->bottom - rect->top) - surf[surf_no].h) : rect->bottom;
			
			for (int fx = renderRect.left; fx < renderRect.right; fx++)
			{
				for (int fy = renderRect.top; fy < renderRect.bottom; fy++)
				{
					int dx = (x + 5 * i) + (fx - rect->left);
					int dy = y + (fy - rect->top);
					
					BUFFER_PIXEL pixel = surf[SURFACE_ID_FONT].data[fy * surf[SURFACE_ID_FONT].w + fx];
					if (pixel.y <= 16)
						continue;
					SET_BUFFER_PIXEL(surf[surf_no].data, surf[surf_no].w, dx, dy, colPixel.y, colPixel.u, colPixel.v);
				}
			}
		}
	}
}

void EndTextObject()
{
	ReleaseSurface(SURFACE_ID_FONT);
}
