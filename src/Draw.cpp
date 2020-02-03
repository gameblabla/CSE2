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

BOOL gb60fps;
BOOL gbSmoothScrolling;

static Backend_Surface *framebuffer;

static Backend_Surface *surf[SURFACE_ID_MAX];

static FontObject *font;

// This doesn't exist in the Linux port, so none of these symbol names are accurate
static struct
{
	char name[50];
	unsigned int width;
	unsigned int height;
	SurfaceType type;
	BOOL bSystem;	// Basically a 'do not regenerate' flag
} surface_metadata[SURFACE_ID_MAX];

static BOOL gbVsync;

BOOL Flip_SystemTask(void)
{
	static unsigned long timePrev;
	static unsigned long timeNow;

	if (gbVsync)
	{
		if (!SystemTask())
			return FALSE;
	}
	else
	{
		const unsigned int frameDelays[3] = {17, 16, 17};
		static unsigned int frame;

		const unsigned int delay = gb60fps ? frameDelays[frame % 3] : 20;
		++frame;

		while (TRUE)
		{
			if (!SystemTask())
				return FALSE;

			// Framerate limiter
			timeNow = SDL_GetTicks();

			if (SDL_TICKS_PASSED(timeNow, timePrev + delay))
				break;

			SDL_Delay(1);
		}

		if (SDL_TICKS_PASSED(timeNow, timePrev + 100))
			timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
		else
			timePrev += delay;
	}

	Backend_DrawScreen();

	if (RestoreSurfaces())
	{
		RestoreStripper();
		RestoreMapName();
		RestoreTextScript();
	}

	return TRUE;
}

BOOL StartDirectDraw(const char *title, int lMagnification, BOOL b60fps, BOOL bSmoothScrolling, BOOL bVsync)
{
	puts("Available SDL2 video drivers:");

	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
		puts(SDL_GetVideoDriver(i));

	printf("Selected SDL2 video driver: %s\n", SDL_GetCurrentVideoDriver());

	gb60fps = b60fps;
	gbSmoothScrolling = bSmoothScrolling;

	memset(surface_metadata, 0, sizeof(surface_metadata));

	switch (lMagnification)
	{
		default:
			magnification = lMagnification;
			fullscreen = FALSE;
			break;

		case 0:
			SDL_DisplayMode display_mode;
			if (SDL_GetDesktopDisplayMode(0, &display_mode) < 0)
				return FALSE;

			magnification = display_mode.w / WINDOW_WIDTH < display_mode.h / WINDOW_HEIGHT ? display_mode.w / WINDOW_WIDTH : display_mode.h / WINDOW_HEIGHT;
			fullscreen = TRUE;
			break;
	}

	// Ugly way to round the magnification up to the nearest multiple of SPRITE_SCALE (we can't use 2x sprites at 1x or 3x internal resolution)
	magnification = ((magnification + (SPRITE_SCALE - 1)) / SPRITE_SCALE) * SPRITE_SCALE;

	// If v-sync is requested, check if it's available
	if (bVsync)
	{
		SDL_DisplayMode display_mode;
		if (SDL_GetDesktopDisplayMode(0, &display_mode) == 0)
			gbVsync = display_mode.refresh_rate == (b60fps ? 60 : 50);
	}

	framebuffer = Backend_Init(title, WINDOW_WIDTH * magnification, WINDOW_HEIGHT * magnification, fullscreen, gbVsync);

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
	const int magnification_scaled = magnification / SPRITE_SCALE;

	unsigned int pitch;
	unsigned char *pixels = Backend_LockSurface(surf[surf_no], &pitch, width * magnification_scaled, height * magnification_scaled);

	if (magnification_scaled == 1)
	{
		// Just copy the pixels the way they are
		for (int y = 0; y < height; ++y)
		{
			const unsigned char *src_row = &image_buffer[y * width * 4];
			unsigned char *dst_row = &pixels[y * pitch];

			memcpy(dst_row, src_row, width * 4);
		}
	}
	else
	{
		// Upscale the bitmap to the game's internal resolution
		for (int y = 0; y < height; ++y)
		{
			const unsigned char *src_row = &image_buffer[y * width * 4];
			unsigned char *dst_row = &pixels[y * pitch * magnification_scaled];

			const unsigned char *src_ptr = src_row;
			unsigned char *dst_ptr = dst_row;

			for (int x = 0; x < width; ++x)
			{
				for (int i = 0; i < magnification_scaled; ++i)
				{
					*dst_ptr++ = src_ptr[0];
					*dst_ptr++ = src_ptr[1];
					*dst_ptr++ = src_ptr[2];
					*dst_ptr++ = src_ptr[3];
				}

				src_ptr += 4;
			}

			for (int i = 1; i < magnification_scaled; ++i)
				memcpy(dst_row + i * pitch, dst_row, width * magnification_scaled * 4);
		}
	}

	Backend_UnlockSurface(surf[surf_no], width * magnification_scaled, height * magnification_scaled);

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

	unsigned int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height, FALSE);

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

	unsigned int width, height;
	unsigned char *image_buffer = NULL;

	const char *file_extensions[] = {"pbm", "bmp", "png"};
	for (size_t i = 0; i < sizeof(file_extensions) / sizeof(file_extensions[0]); ++i)
	{
		sprintf(path, "%s/%s.%s", gDataPath, name, file_extensions[i]);

		image_buffer = DecodeBitmapFromFile(path, &width, &height, TRUE);

		if (image_buffer != NULL)
			break;
	}

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

	unsigned int width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height, FALSE);

	if (image_buffer == NULL)
		return FALSE;

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

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)
#endif
	{
		PrintBitmapError("surface no", surf_no);
		return FALSE;
	}

	unsigned int width, height;
	unsigned char *image_buffer = NULL;

	const char *file_extensions[] = {"pbm", "bmp", "png"};
	for (size_t i = 0; i < sizeof(file_extensions) / sizeof(file_extensions[0]); ++i)
	{
		sprintf(path, "%s/%s.%s", gDataPath, name, file_extensions[i]);

		image_buffer = DecodeBitmapFromFile(path, &width, &height, TRUE);

		if (image_buffer != NULL)
			break;
	}

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

static void ScaleRect(const RECT *rect, RECT *scaled_rect)
{
	scaled_rect->left = rect->left * magnification;
	scaled_rect->top = rect->top * magnification;
	scaled_rect->right = rect->right * magnification;
	scaled_rect->bottom = rect->bottom * magnification;
}

void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // Transparency
{
	static RECT src_rect;
	ScaleRect(rect, &src_rect);

	static RECT rcViewScaled;
	ScaleRect(rcView, &rcViewScaled);

	if (x + src_rect.right - src_rect.left > rcViewScaled.right)
		src_rect.right -= (x + src_rect.right - src_rect.left) - rcViewScaled.right;

	if (x < rcViewScaled.left)
	{
		src_rect.left += rcViewScaled.left - x;
		x = rcViewScaled.left;
	}

	if (y + src_rect.bottom - src_rect.top > rcViewScaled.bottom)
		src_rect.bottom -= (y + src_rect.bottom - src_rect.top) - rcViewScaled.bottom;

	if (y < rcViewScaled.top)
	{
		src_rect.top += rcViewScaled.top - y;
		y = rcViewScaled.top;
	}

	Backend_Blit(surf[surf_no], &src_rect, framebuffer, x, y, TRUE);
}

void PutBitmap4(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // No Transparency
{
	static RECT src_rect;
	ScaleRect(rect, &src_rect);

	static RECT rcViewScaled;
	ScaleRect(rcView, &rcViewScaled);

	if (x + src_rect.right - src_rect.left > rcViewScaled.right)
		src_rect.right -= (x + src_rect.right - src_rect.left) - rcViewScaled.right;

	if (x < rcViewScaled.left)
	{
		src_rect.left += rcViewScaled.left - x;
		x = rcViewScaled.left;
	}

	if (y + src_rect.bottom - src_rect.top > rcViewScaled.bottom)
		src_rect.bottom -= (y + src_rect.bottom - src_rect.top) - rcViewScaled.bottom;

	if (y < rcViewScaled.top)
	{
		src_rect.top += rcViewScaled.top - y;
		y = rcViewScaled.top;
	}

	Backend_Blit(surf[surf_no], &src_rect, framebuffer, x, y, FALSE);
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

	Backend_ColourFill(framebuffer, &dst_rect, red, green, blue, 0xFF);
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
	const unsigned char alpha = (col >> 24) & 0xFF;

	Backend_ColourFill(surf[surf_no], &dst_rect, red, green, blue, alpha);
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

int SubpixelToScreenCoord(int coord)
{
	if (gbSmoothScrolling)
		return (coord * magnification) / 0x200;
	else
		return (coord / (0x200 / SPRITE_SCALE)) * (magnification / SPRITE_SCALE);
}

int PixelToScreenCoord(int coord)
{
	return coord * magnification;
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
