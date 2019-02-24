#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gccore.h>

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
typedef struct YUVColour
{
	uint8_t y;
	uint8_t cb;
	uint8_t cr;
} YUVColour;

YUVColour RGBToYUV(uint8_t red, uint8_t green, uint8_t blue)
{
	YUVColour yuv;
	yuv.y = (299 * red + 587 * green + 114 * blue) / 1000;
	yuv.cb = (-16874 * red - 33126 * green + 50000 * blue + 12800000) / 100000;
	yuv.cr = (50000 * red - 41869 * green - 8131 * blue + 12800000) / 100000;
	return yuv;
}

#define SET_BUFFER_PIXEL(buffer, w, x, y, sr, sg, sb)	buffer[y * w + x].r = sr; \
														buffer[y * w + x].g = sg; \
														buffer[y * w + x].b = sb;

//Frame-buffers and screen mode
uint32_t *xfb[2];
BUFFER_PIXEL *screenBuffer;

bool currentFramebuffer;
GXRModeObj *prefMode;

//Draw to screen
BOOL Flip_SystemTask()
{
	//Update inputs
	UpdateInput();
	
	if (gKey & KEY_ESCAPE)
		return FALSE;
	
	//Write to framebuffer
	uint32_t *pointer = xfb[currentFramebuffer];
	
	static uint8_t r = 0;
	r += 2;
	static uint8_t g = 0;
	g += 3;
	static uint8_t b = 0;
	b += 4;
	
	for (unsigned int y = 0; y < WINDOW_HEIGHT; y++)
	{
		for (unsigned int x = 0; x < WINDOW_WIDTH; x += 2)
		{
			YUVColour colour1 = RGBToYUV(screenBuffer[y * WINDOW_WIDTH + x].r, screenBuffer[y * WINDOW_WIDTH + x].g, screenBuffer[y * WINDOW_WIDTH + x].b);
			YUVColour colour2 = RGBToYUV(screenBuffer[y * WINDOW_WIDTH + x + 1].r, screenBuffer[y * WINDOW_WIDTH + x + 1].g, screenBuffer[y * WINDOW_WIDTH + x + 1].b);
			*pointer++ = (colour1.y << 24) | (((colour1.cb + colour2.cb) >> 1) << 16) | (colour2.y << 8) | ((colour1.cr + colour2.cr) >> 1);
		}
		
		memcpy(pointer, pointer - (WINDOW_WIDTH / 2), (WINDOW_WIDTH / 2) * sizeof(*pointer));
		pointer += WINDOW_WIDTH / 2;
	}
	
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
	
	//Allocate our personal framebuffer
	screenBuffer = (BUFFER_PIXEL*)malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(BUFFER_PIXEL));
	
	//Setup the video registers with the chosen mode
	VIDEO_Configure(prefMode);
	
	//Tell the VDP where are frame-buffer is
	VIDEO_SetNextFramebuffer(xfb);
	
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
	//Free our screenbuffer
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

static BOOL LoadBitmap_File(const char *name, Surface_Ids surf_no, bool create_surface)
{
	/*
	char path[PATH_LENGTH];
	SDL_RWops *fp;
	
	//Attempt to load PBM
	sprintf(path, "%s/%s.pbm", gDataPath, name);
	fp = SDL_RWFromFile(path, "rb");
	if (fp)
	{
		if (!IsEnableBitmap(fp))
		{
			printf("Tried to load bitmap to surface %d, but it's missing the '(C)Pixel' string\n", surf_no);
			fp->close(fp);
		}
		else
		{
			printf("Loading surface (as .pbm) from %s for surface id %d\n", path, surf_no);
			if (LoadBitmap(fp, surf_no, create_surface))
				return TRUE;
		}
	}
	
	//Attempt to load BMP
	sprintf(path, "%s/%s.bmp", gDataPath, name);
	fp = SDL_RWFromFile(path, "rb");
	if (fp)
	{
		printf("Loading surface (as .bmp) from %s for surface id %d\n", path, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return TRUE;
	}
	
	printf("Failed to open file %s\n", name);
	return FALSE;
	*/
	return TRUE;
}

static BOOL LoadBitmap_Resource(const char *res, Surface_Ids surf_no, bool create_surface)
{
	/*
	SDL_RWops *fp = FindResource(res);
	
	if (fp)
	{
		printf("Loading surface from resource %s for surface id %d\n", res, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return TRUE;
	}
	
	printf("Failed to open resource %s\n", res);
	return FALSE;
	*/
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
	/*
	//Get renderer size
	int w, h;
	SDL_GetRendererOutputSize(gRenderer, &w, &h);

	//Get texture of what's currently rendered on screen
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
	SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

	//Get rects
	SDL_Rect frameRect = RectToSDLRectScaled(rect);

	SDL_BlitSurface(surface, &frameRect, surf[surf_no].surface, &frameRect);
	surf[surf_no].needs_updating = true;

	//Free surface
	SDL_FreeSurface(surface);
	*/
}

static void DrawBitmap(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no, bool transparent)
{
	const unsigned char col_red = 255;
	const unsigned char col_green = surf_no * 0x20;
	const unsigned char col_blue = surf_no * 0x50;
	
	for (int fx = rect->left; fx < rect->right; fx++)
	{
		for (int fy = rect->top; fy < rect->bottom; fy++)
		{
			int dx = x + (fx - rect->left);
			int dy = y + (fy - rect->top);
			
			if (dx < rcView->left || y < rcView->top)
				continue;
			if (dx < 0 || dy < 0)
				continue;
			
			if (dx >= rcView->right || y >= rcView->bottom)
				break;
			if (dx >= WINDOW_WIDTH || dy >= WINDOW_HEIGHT)
				break;
			
			SET_BUFFER_PIXEL(screenBuffer, WINDOW_WIDTH, dx, dy, col_red, col_green, col_blue);
		}
	}
	
	/*
	if (surf[surf_no].needs_updating)
	{
		FlushSurface(surf_no);
		surf[surf_no].needs_updating = false;
	}

	//Get SDL_Rects
	SDL_Rect clipRect = RectToSDLRectScaled(rcView);

	SDL_Rect frameRect = RectToSDLRectScaled(rect);
	
	//Get dest rect
	SDL_Rect destRect = {x * magnification, y * magnification, frameRect.w, frameRect.h};
	
	//Set cliprect
	SDL_RenderSetClipRect(gRenderer, &clipRect);
	
	SDL_SetTextureBlendMode(surf[surf_no].texture, transparent ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

	//Draw to screen
	if (SDL_RenderCopy(gRenderer, surf[surf_no].texture, &frameRect, &destRect) < 0)
		printf("Failed to draw texture %d\nSDL Error: %s\n", surf_no, SDL_GetError());
	
	//Undo cliprect
	SDL_RenderSetClipRect(gRenderer, NULL);
	*/
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
	/*
	//Get rects
	SDL_Rect rcSet = {x * magnification, y * magnification, (rect->right - rect->left) * magnification, (rect->bottom - rect->top) * magnification};
	SDL_Rect frameRect = RectToSDLRectScaled(rect);

	SDL_BlitSurface(surf[from].surface, &frameRect, surf[to].surface, &rcSet);
	surf[to].needs_updating = true;
	*/
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
	
	for (int x = rect->left; x < rect->right; x++)
	{
		for (int y = rect->top; y < rect->bottom; y++)
		{
			if (x < 0 || y < 0)
				continue;
			if (x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT)
				break;
			SET_BUFFER_PIXEL(screenBuffer, WINDOW_WIDTH, x, y, col_red, col_green, col_blue);
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
		
		for (int x = rect->left; x < rect->right; x++)
		{
			for (int y = rect->top; y < rect->bottom; y++)
			{
				if (x < 0 || y < 0)
					continue;
				if (x >= surf[surf_no].w || y >= surf[surf_no].h)
					break;
				SET_BUFFER_PIXEL(surf[surf_no].data, surf[surf_no].w, x, y, col_red, col_green, col_blue);
			}
		}
	}
}

void InitTextObject()
{
	/*
	//Get font size
	unsigned int fontWidth, fontHeight;
	fontWidth = 5 * magnification;
	fontHeight = 10 * magnification;
	
	//Open Font.ttf
	char path[PATH_LENGTH];
#ifdef JAPANESE
	sprintf(path, "%s/font/msgothic.ttc", gModulePath);
#else
	sprintf(path, "%s/font/cour.ttf", gModulePath);
#endif

	gFont = LoadFont(path, fontWidth, fontHeight);
	*/
}

void PutText(int x, int y, const char *text, uint32_t color)
{
	/*
	int surface_width, surface_height;
	SDL_GetRendererOutputSize(gRenderer, &surface_width, &surface_height);

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, surface_width, surface_height, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

	DrawText(gFont, surface, x * magnification, y * magnification, color, text, strlen(text));

	SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer, screen_texture, NULL, NULL);
	SDL_DestroyTexture(screen_texture);
	*/
}

void PutText2(int x, int y, const char *text, uint32_t color, Surface_Ids surf_no)
{
	/*
	DrawText(gFont, surf[surf_no].surface, x * magnification, y * magnification, color, text, strlen(text));
	surf[surf_no].needs_updating = true;
	*/
}

void EndTextObject()
{
	/*
	//Destroy font
	UnloadFont(gFont);
	gFont = NULL;
	*/
}
