#include "Draw.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "Backends/Rendering.h"
#include "Bitmap.h"
#include "CommonDefines.h"
#include "Ending.h"
#include "File.h"
#include "Font.h"
#include "Generic.h"
#include "Main.h"
#include "MapName.h"
#include "Resource.h"
#include "TextScr.h"

typedef enum SurfaceType
{
	SURFACE_SOURCE_NONE = 1,
	SURFACE_SOURCE_RESOURCE,
	SURFACE_SOURCE_FILE
} SurfaceType;

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

int magnification;
BOOL fullscreen;

static Backend_Surface *framebuffer;

static Backend_Surface *surf[SURFACE_ID_MAX];

static FontObject *font;

// This doesn't exist in the Linux port, so none of these symbol names are accurate
static struct
{
	char name[20];
	unsigned int width;
	unsigned int height;
	SurfaceType type;
	BOOL bSystem;	// Basically a 'do not regenerate' flag
} surface_metadata[SURFACE_ID_MAX];

BOOL Flip_SystemTask(void)
{
	static unsigned long timePrev;
	static unsigned long timeNow;

	while (TRUE)
	{
		if (!SystemTask())
			return FALSE;

		// Framerate limiter
		timeNow = SDL_GetTicks();

		if (SDL_TICKS_PASSED(timeNow, timePrev + 20))
			break;

		SDL_Delay(1);
	}

	if (SDL_TICKS_PASSED(timeNow, timePrev + 100))
		timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
	else
		timePrev += 20;

	Backend_DrawScreen();

	if (RestoreSurfaces())
	{
		RestoreStripper();
		RestoreMapName();
		RestoreTextScript();
	}

	return TRUE;
}

BOOL StartDirectDraw(const char *title, int width, int height, int lMagnification)
{
#ifndef NDEBUG
	puts("Available SDL2 video drivers:");

	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
		puts(SDL_GetVideoDriver(i));

	printf("Selected SDL2 video driver: %s\n", SDL_GetCurrentVideoDriver());
#endif

	memset(surface_metadata, 0, sizeof(surface_metadata));

	switch (lMagnification)
	{
		case 0:
			magnification = 1;
			fullscreen = FALSE;
			break;

		case 1:
			magnification = 2;
			fullscreen = FALSE;
			break;

		case 2:
			magnification = 2;
			fullscreen = TRUE;
			break;
	}

	framebuffer = Backend_Init(title, width, height, fullscreen);

	if (framebuffer == NULL)
		return FALSE;

	return TRUE;
}

void EndDirectDraw(void)
{
	int i;

	// Release all surfaces
	for (i = 0; i < SURFACE_ID_MAX; ++i)
	{
		if (surf[i] != NULL)
		{
			Backend_FreeSurface(surf[i]);
			surf[i] = NULL;
		}
	}

	framebuffer = NULL;

	Backend_Deinit();

	memset(surface_metadata, 0, sizeof(surface_metadata));
}

void ReleaseSurface(SurfaceID s)
{
	// Release the surface we want to release
	if (surf[s] != NULL)
	{
		Backend_FreeSurface(surf[s]);
		surf[s] = NULL;
	}

	memset(&surface_metadata[s], 0, sizeof(surface_metadata[0]));
}

static BOOL ScaleAndUploadSurface(const unsigned char *image_buffer, int width, int height, SurfaceID surf_no)
{
	// IF YOU WANT TO ADD HD SPRITES, THIS IS THE CODE YOU SHOULD EDIT
	unsigned int pitch;
	unsigned char *pixels = Backend_LockSurface(surf[surf_no], &pitch, width * magnification, height * magnification);

	if (magnification == 1)
	{
		// Just copy the pixels the way they are
		for (int y = 0; y < height; ++y)
		{
			const unsigned char *src_row = &image_buffer[y * width * 3];
			unsigned char *dst_row = &pixels[y * pitch];

			memcpy(dst_row, src_row, width * 3);
		}
	}
	else
	{
		// Upscale the bitmap to the game's internal resolution
		for (int y = 0; y < height; ++y)
		{
			const unsigned char *src_row = &image_buffer[y * width * 3];
			unsigned char *dst_row = &pixels[y * pitch * magnification];

			const unsigned char *src_ptr = src_row;
			unsigned char *dst_ptr = dst_row;

			for (int x = 0; x < width; ++x)
			{
				for (int i = 0; i < magnification; ++i)
				{
					*dst_ptr++ = src_ptr[0];
					*dst_ptr++ = src_ptr[1];
					*dst_ptr++ = src_ptr[2];
				}

				src_ptr += 3;
			}

			for (int i = 1; i < magnification; ++i)
				memcpy(dst_row + i * pitch, dst_row, width * magnification * 3);
		}
	}

	Backend_UnlockSurface(surf[surf_no], width * magnification, height * magnification);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL MakeSurface_Resource(const char *name, SurfaceID surf_no)
{
	if (surf_no >= SURFACE_ID_MAX)
		return FALSE;

	if (surf[surf_no] != NULL)
		return FALSE;

	size_t size;
	const unsigned char *data = FindResource(name, "BITMAP", &size);

	if (data == NULL)
		return FALSE;

	int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height);

	if (image_buffer == NULL)
		return FALSE;

	surf[surf_no] = Backend_CreateSurface(width * magnification, height * magnification);

	if (surf[surf_no] == NULL)
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		Backend_FreeSurface(surf[surf_no]);
		FreeBitmap(image_buffer);
		return FALSE;
	}

	surface_metadata[surf_no].type = SURFACE_SOURCE_RESOURCE;
	surface_metadata[surf_no].width = width;
	surface_metadata[surf_no].height = height;
	surface_metadata[surf_no].bSystem = FALSE;
	strcpy(surface_metadata[surf_no].name, name);
	FreeBitmap(image_buffer);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL MakeSurface_File(const char *name, SurfaceID surf_no)
{
	char path[MAX_PATH];
	sprintf(path, "%s/%s.pbm", gDataPath, name);

	if (!IsEnableBitmap(path))
	{
		PrintBitmapError(path, 0);
		return FALSE;
	}

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)
#endif
	{
		PrintBitmapError("surface no", surf_no);
		return FALSE;
	}

	if (surf[surf_no] != NULL)
	{
		PrintBitmapError("existing", surf_no);
		return FALSE;
	}

	size_t size;
	unsigned char *data = LoadFileToMemory(path, &size);

	if (data == NULL)
	{
		PrintBitmapError(path, 1);
		return FALSE;
	}

	int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height);

	if (image_buffer == NULL)
	{
		PrintBitmapError(path, 1);
		return FALSE;
	}

	surf[surf_no] = Backend_CreateSurface(width * magnification, height * magnification);

	if (surf[surf_no] == NULL)
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		Backend_FreeSurface(surf[surf_no]);
		FreeBitmap(image_buffer);
		return FALSE;
	}

	surface_metadata[surf_no].type = SURFACE_SOURCE_FILE;
	surface_metadata[surf_no].width = width;
	surface_metadata[surf_no].height = height;
	surface_metadata[surf_no].bSystem = FALSE;
	strcpy(surface_metadata[surf_no].name, name);
	FreeBitmap(image_buffer);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL ReloadBitmap_Resource(const char *name, SurfaceID surf_no)
{
	if (surf_no >= SURFACE_ID_MAX)
		return FALSE;

	size_t size;
	const unsigned char *data = FindResource(name, "BITMAP", &size);

	int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height);

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	FreeBitmap(image_buffer);

	surface_metadata[surf_no].type = SURFACE_SOURCE_RESOURCE;
	strcpy(surface_metadata[surf_no].name, name);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL ReloadBitmap_File(const char *name, SurfaceID surf_no)
{
	char path[MAX_PATH];
	sprintf(path, "%s/%s.pbm", gDataPath, name);

	if (!IsEnableBitmap(path))
	{
		PrintBitmapError(path, 0);
		return FALSE;
	}

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)
#endif
	{
		PrintBitmapError("surface no", surf_no);
		return FALSE;
	}

	size_t size;
	unsigned char *data = LoadFileToMemory(path, &size);

	if (data == NULL)
	{
		PrintBitmapError(path, 1);
		return FALSE;
	}

	int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height);

	if (image_buffer == NULL)
	{
		PrintBitmapError(path, 1);
		return FALSE;
	}

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	FreeBitmap(image_buffer);
	surface_metadata[surf_no].type = SURFACE_SOURCE_FILE;
	strcpy(surface_metadata[surf_no].name, name);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL MakeSurface_Generic(int bxsize, int bysize, SurfaceID surf_no, BOOL bSystem)
{
#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)	// OOPS (should be '>=')
#endif
		return FALSE;

	if (surf[surf_no] != NULL)
		return FALSE;

	surf[surf_no] = Backend_CreateSurface(bxsize * magnification, bysize * magnification);

	if (surf[surf_no] == NULL)
		return FALSE;

	surface_metadata[surf_no].type = SURFACE_SOURCE_NONE;
	surface_metadata[surf_no].width = bxsize;
	surface_metadata[surf_no].height = bysize;

	if (bSystem)
		surface_metadata[surf_no].bSystem = TRUE;
	else
		surface_metadata[surf_no].bSystem = FALSE;

	strcpy(surface_metadata[surf_no].name, "generic");

	return TRUE;
}

void BackupSurface(SurfaceID surf_no, const RECT *rect)
{
	static RECT scaled_rect;
	scaled_rect.left = rect->left * magnification;
	scaled_rect.top = rect->top * magnification;
	scaled_rect.right = rect->right * magnification;
	scaled_rect.bottom = rect->bottom * magnification;

	Backend_Blit(framebuffer, &scaled_rect, surf[surf_no], scaled_rect.left, scaled_rect.top, FALSE);
}

void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // Transparency
{
	static RECT src_rect;

	src_rect = *rect;

	if (x + rect->right - rect->left > rcView->right)
		src_rect.right -= (x + rect->right - rect->left) - rcView->right;

	if (x < rcView->left)
	{
		src_rect.left += rcView->left - x;
		x = rcView->left;
	}

	if (y + rect->bottom - rect->top > rcView->bottom)
		src_rect.bottom -= (y + rect->bottom - rect->top) - rcView->bottom;

	if (y < rcView->top)
	{
		src_rect.top += rcView->top - y;
		y = rcView->top;
	}

	src_rect.left *= magnification;
	src_rect.top *= magnification;
	src_rect.right *= magnification;
	src_rect.bottom *= magnification;

	Backend_Blit(surf[surf_no], &src_rect, framebuffer, x * magnification, y * magnification, TRUE);
}

void PutBitmap4(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // No Transparency
{
	static RECT src_rect;

	src_rect = *rect;

	if (x + rect->right - rect->left > rcView->right)
		src_rect.right -= (x + rect->right - rect->left) - rcView->right;

	if (x < rcView->left)
	{
		src_rect.left += rcView->left - x;
		x = rcView->left;
	}

	if (y + rect->bottom - rect->top > rcView->bottom)
		src_rect.bottom -= (y + rect->bottom - rect->top) - rcView->bottom;

	if (y < rcView->top)
	{
		src_rect.top += rcView->top - y;
		y = rcView->top;
	}

	src_rect.left *= magnification;
	src_rect.top *= magnification;
	src_rect.right *= magnification;
	src_rect.bottom *= magnification;

	Backend_Blit(surf[surf_no], &src_rect, framebuffer, x * magnification, y * magnification, FALSE);
}

void Surface2Surface(int x, int y, const RECT *rect, int to, int from)
{
	static RECT src_rect;

	src_rect.left = rect->left * magnification;
	src_rect.top = rect->top * magnification;
	src_rect.right = rect->right * magnification;
	src_rect.bottom = rect->bottom * magnification;

	Backend_Blit(surf[from], &src_rect, surf[to], x * magnification, y * magnification, TRUE);
}

unsigned long GetCortBoxColor(unsigned long col)
{
	// Comes in 00BBGGRR, goes out 00BBGGRR
	return col;
}

void CortBox(const RECT *rect, unsigned long col)
{
	static RECT dst_rect;
	dst_rect.left = rect->left * magnification;
	dst_rect.top = rect->top * magnification;
	dst_rect.right = rect->right * magnification;
	dst_rect.bottom = rect->bottom * magnification;

	const unsigned char red = col & 0xFF;
	const unsigned char green = (col >> 8) & 0xFF;
	const unsigned char blue = (col >> 16) & 0xFF;

	Backend_ColourFill(framebuffer, &dst_rect, red, green, blue);
}

void CortBox2(const RECT *rect, unsigned long col, SurfaceID surf_no)
{
	static RECT dst_rect;
	dst_rect.left = rect->left * magnification;
	dst_rect.top = rect->top * magnification;
	dst_rect.right = rect->right * magnification;
	dst_rect.bottom = rect->bottom * magnification;

	surface_metadata[surf_no].type = SURFACE_SOURCE_NONE;

	const unsigned char red = col & 0xFF;
	const unsigned char green = (col >> 8) & 0xFF;
	const unsigned char blue = (col >> 16) & 0xFF;

	Backend_ColourFill(surf[surf_no], &dst_rect, red, green, blue);
}

BOOL DummiedOutLogFunction(int unknown)
{
	char unknown2[0x100];
	int unknown3;
	int unknown4;
	int unknown5;

	(void)unknown;
	(void)unknown2;
	(void)unknown3;
	(void)unknown4;
	(void)unknown5;

	return TRUE;
}

int RestoreSurfaces(void)	// Guessed function name - this doesn't exist in the Linux port
{
	int s;
	RECT rect;
	int surfaces_regenerated = 0;

	if (framebuffer == NULL)
		return surfaces_regenerated;

	if (Backend_IsSurfaceLost(framebuffer))
	{
		++surfaces_regenerated;
		Backend_RestoreSurface(framebuffer);
		DummiedOutLogFunction(0x62);
	}

	for (s = 0; s < SURFACE_ID_MAX; ++s)
	{
		if (surf[s] != NULL)
		{
			if (Backend_IsSurfaceLost(surf[s]))
			{
				++surfaces_regenerated;
				Backend_RestoreSurface(surf[s]);
				DummiedOutLogFunction(0x30 + s);

				if (!surface_metadata[s].bSystem)
				{
					switch (surface_metadata[s].type)
					{
						case SURFACE_SOURCE_NONE:
							rect.left = 0;
							rect.top = 0;
							rect.right = surface_metadata[s].width;
							rect.bottom = surface_metadata[s].height;
							CortBox2(&rect, 0, (SurfaceID)s);
							break;

						case SURFACE_SOURCE_RESOURCE:
							ReloadBitmap_Resource(surface_metadata[s].name, (SurfaceID)s);
							break;

						case SURFACE_SOURCE_FILE:
							ReloadBitmap_File(surface_metadata[s].name, (SurfaceID)s);
							break;
					}
				}
			}
		}
	}

	return surfaces_regenerated;
}

// TODO - Inaccurate stack frame
void InitTextObject(const char *name)
{
	(void)name;	// Unused in this branch

	size_t size;
	const unsigned char *data = FindResource("FONT", "FONT", &size);

	font = LoadFontFromData(data, size, 8 * magnification, 9 * magnification);
}

void PutText(int x, int y, const char *text, unsigned long color)
{
	DrawText(font, framebuffer, x * magnification, y * magnification, color, text);
}

void PutText2(int x, int y, const char *text, unsigned long color, SurfaceID surf_no)
{
	DrawText(font, surf[surf_no], x * magnification, y * magnification, color, text);
}

void EndTextObject(void)
{
	UnloadFont(font);
}
