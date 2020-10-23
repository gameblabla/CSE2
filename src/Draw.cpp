// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include "Draw.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifdef _3DS
 #include <3ds.h>
#endif

#include "WindowsWrapper.h"

#include "Backends/Misc.h"
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

static int mag;
static BOOL fullscreen;	// TODO - Not the original variable name

static RenderBackend_Surface *framebuffer;	// TODO - Not the original variable name

static RenderBackend_Surface *surf[SURFACE_ID_MAX];

static Font *font;	// TODO - Not the original variable name

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
	// TODO - Not the original variable names
	static unsigned long timePrev;
	static unsigned long timeNow;

	while (TRUE)
	{
		if (!SystemTask())
			return FALSE;

		// Framerate limiter
		timeNow = Backend_GetTicks();

		if (timeNow >= timePrev + 20)
			break;

		Backend_Delay(1);
	}

	if (timeNow >= timePrev + 100)
		timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
	else
		timePrev += 20;

	RenderBackend_DrawScreen();

#ifdef _3DS
	// This would go in Backend_SystemTask, but that causes a hang
	// because of a race condition: aptMainLoop cannot be called
	// between C3D_FrameBegin and C3D_FrameEnd
	if (!aptMainLoop())
		return false;
#endif

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
	memset(surface_metadata, 0, sizeof(surface_metadata));

	switch (lMagnification)
	{
		case 0:
			mag = 1;
			fullscreen = FALSE;
			break;

		case 1:
			mag = 2;
			fullscreen = FALSE;
			break;

		case 2:
			mag = 2;
			fullscreen = TRUE;
			break;
	}

	framebuffer = RenderBackend_Init(title, width, height, fullscreen);

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
			RenderBackend_FreeSurface(surf[i]);
			surf[i] = NULL;
		}
	}

	framebuffer = NULL;

	RenderBackend_Deinit();

	memset(surface_metadata, 0, sizeof(surface_metadata));
}

void ReleaseSurface(SurfaceID s)
{
	// Release the surface we want to release
	if (surf[s] != NULL)
	{
		RenderBackend_FreeSurface(surf[s]);
		surf[s] = NULL;
	}

	memset(&surface_metadata[s], 0, sizeof(surface_metadata[0]));
}

static BOOL ScaleAndUploadSurface(const unsigned char *image_buffer, size_t width, size_t height, SurfaceID surf_no)
{
	// IF YOU WANT TO ADD HD SPRITES, THIS IS THE CODE YOU SHOULD EDIT
	if (mag == 1)
	{
		// Just copy the pixels the way they are
		RenderBackend_UploadSurface(surf[surf_no], image_buffer, width, height);
	}
	else
	{
		unsigned char *upscaled_image_buffer = (unsigned char*)malloc(width * mag * height * mag * 3);

		if (upscaled_image_buffer == NULL)
			return FALSE;

		// Upscale the bitmap to the game's internal resolution
		for (size_t y = 0; y < height; ++y)
		{
			const unsigned char *src_row = &image_buffer[y * width * 3];
			unsigned char *dst_row = &upscaled_image_buffer[y * mag * width * mag * 3];

			const unsigned char *src_ptr = src_row;
			unsigned char *dst_ptr = dst_row;

			for (size_t x = 0; x < width; ++x)
			{
				for (int i = 0; i < mag; ++i)
				{
					*dst_ptr++ = src_ptr[0];
					*dst_ptr++ = src_ptr[1];
					*dst_ptr++ = src_ptr[2];
				}

				src_ptr += 3;
			}

			for (int i = 1; i < mag; ++i)
				memcpy(dst_row + i * width * mag * 3, dst_row, width * mag * 3);
		}

		RenderBackend_UploadSurface(surf[surf_no], upscaled_image_buffer, width * mag, height * mag);

		free(upscaled_image_buffer);
	}

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

	size_t width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height, 3);

	if (image_buffer == NULL)
		return FALSE;

	surf[surf_no] = RenderBackend_CreateSurface(width * mag, height * mag, false);

	if (surf[surf_no] == NULL)
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		RenderBackend_FreeSurface(surf[surf_no]);
		FreeBitmap(image_buffer);
		return FALSE;
	}

	FreeBitmap(image_buffer);

	surface_metadata[surf_no].type = SURFACE_SOURCE_RESOURCE;
	surface_metadata[surf_no].width = width;
	surface_metadata[surf_no].height = height;
	surface_metadata[surf_no].bSystem = FALSE;
	strcpy(surface_metadata[surf_no].name, name);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL MakeSurface_File(const char *name, SurfaceID surf_no)
{
	std::string path = gDataPath + '/' + name + ".pbm";

	if (!IsEnableBitmap(path.c_str()))
	{
		ErrorLog(path.c_str(), 0);
		return FALSE;
	}

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)
#endif
	{
		ErrorLog("surface no", surf_no);
		return FALSE;
	}

	if (surf[surf_no] != NULL)
	{
		ErrorLog("existing", surf_no);
		return FALSE;
	}

	size_t width, height;
	unsigned char *image_buffer = DecodeBitmapFromFile(path.c_str(), &width, &height, 3);

	if (image_buffer == NULL)
	{
		ErrorLog(path.c_str(), 1);
		return FALSE;
	}

	surf[surf_no] = RenderBackend_CreateSurface(width * mag, height * mag, false);

	if (surf[surf_no] == NULL)
	{
		FreeBitmap(image_buffer);
		return FALSE;
	}

	if (!ScaleAndUploadSurface(image_buffer, width, height, surf_no))
	{
		RenderBackend_FreeSurface(surf[surf_no]);
		FreeBitmap(image_buffer);
		return FALSE;
	}

	FreeBitmap(image_buffer);

	surface_metadata[surf_no].type = SURFACE_SOURCE_FILE;
	surface_metadata[surf_no].width = width;
	surface_metadata[surf_no].height = height;
	surface_metadata[surf_no].bSystem = FALSE;
	strcpy(surface_metadata[surf_no].name, name);

	return TRUE;
}

// TODO - Inaccurate stack frame
BOOL ReloadBitmap_Resource(const char *name, SurfaceID surf_no)
{
	if (surf_no >= SURFACE_ID_MAX)
		return FALSE;

	size_t size;
	const unsigned char *data = FindResource(name, "BITMAP", &size);

	if (data == NULL)
		return FALSE;

	size_t width, height;
	unsigned char *image_buffer = DecodeBitmap(data, size, &width, &height, 3);

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
	std::string path = gDataPath + '/' + name + ".pbm";

	if (!IsEnableBitmap(path.c_str()))
	{
		ErrorLog(path.c_str(), 0);
		return FALSE;
	}

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)
#endif
	{
		ErrorLog("surface no", surf_no);
		return FALSE;
	}

	size_t width, height;
	unsigned char *image_buffer = DecodeBitmapFromFile(path.c_str(), &width, &height, 3);

	if (image_buffer == NULL)
	{
		ErrorLog(path.c_str(), 1);
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
BOOL MakeSurface_Generic(int bxsize, int bysize, SurfaceID surf_no, BOOL bSystem, BOOL bTarget)
{
#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)	// OOPS (should be '>=')
#endif
		return FALSE;

	if (surf[surf_no] != NULL)
		return FALSE;

	surf[surf_no] = RenderBackend_CreateSurface(bxsize * mag, bysize * mag, bTarget);

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
	if (surf[surf_no] == NULL)
		return;

	static RenderBackend_Rect rcSet;	// TODO - Not the original variable name
	rcSet.left = rect->left * mag;
	rcSet.top = rect->top * mag;
	rcSet.right = rect->right * mag;
	rcSet.bottom = rect->bottom * mag;

	// Do not draw invalid RECTs
	if (rcSet.right <= rcSet.left || rcSet.bottom <= rcSet.top)
		return;

	RenderBackend_Blit(framebuffer, &rcSet, surf[surf_no], rcSet.left, rcSet.top, FALSE);
}

void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // Transparency
{
	if (surf[surf_no] == NULL)
		return;

	static RenderBackend_Rect rcWork;

	rcWork.left = rect->left;
	rcWork.top = rect->top;
	rcWork.right = rect->right;
	rcWork.bottom = rect->bottom;

	if (x + rect->right - rect->left > rcView->right)
		rcWork.right -= (x + rect->right - rect->left) - rcView->right;

	if (x < rcView->left)
	{
		rcWork.left += rcView->left - x;
		x = rcView->left;
	}

	if (y + rect->bottom - rect->top > rcView->bottom)
		rcWork.bottom -= (y + rect->bottom - rect->top) - rcView->bottom;

	if (y < rcView->top)
	{
		rcWork.top += rcView->top - y;
		y = rcView->top;
	}

	rcWork.left *= mag;
	rcWork.top *= mag;
	rcWork.right *= mag;
	rcWork.bottom *= mag;

	// Do not draw invalid RECTs
	if (rcWork.right <= rcWork.left || rcWork.bottom <= rcWork.top)
		return;

	RenderBackend_Blit(surf[surf_no], &rcWork, framebuffer, x * mag, y * mag, TRUE);
}

void PutBitmap4(const RECT *rcView, int x, int y, const RECT *rect, SurfaceID surf_no) // No Transparency
{
	if (surf[surf_no] == NULL)
		return;

	static RenderBackend_Rect rcWork;

	rcWork.left = rect->left;
	rcWork.top = rect->top;
	rcWork.right = rect->right;
	rcWork.bottom = rect->bottom;

	if (x + rect->right - rect->left > rcView->right)
		rcWork.right -= (x + rect->right - rect->left) - rcView->right;

	if (x < rcView->left)
	{
		rcWork.left += rcView->left - x;
		x = rcView->left;
	}

	if (y + rect->bottom - rect->top > rcView->bottom)
		rcWork.bottom -= (y + rect->bottom - rect->top) - rcView->bottom;

	if (y < rcView->top)
	{
		rcWork.top += rcView->top - y;
		y = rcView->top;
	}

	rcWork.left *= mag;
	rcWork.top *= mag;
	rcWork.right *= mag;
	rcWork.bottom *= mag;

	// Do not draw invalid RECTs
	if (rcWork.right <= rcWork.left || rcWork.bottom <= rcWork.top)
		return;

	RenderBackend_Blit(surf[surf_no], &rcWork, framebuffer, x * mag, y * mag, FALSE);
}

void Surface2Surface(int x, int y, const RECT *rect, SurfaceID to, SurfaceID from)
{
	if (surf[to] == NULL || surf[from] == NULL)
		return;

	static RenderBackend_Rect rcWork;

	rcWork.left = rect->left * mag;
	rcWork.top = rect->top * mag;
	rcWork.right = rect->right * mag;
	rcWork.bottom = rect->bottom * mag;

	// Do not draw invalid RECTs
	if (rcWork.right <= rcWork.left || rcWork.bottom <= rcWork.top)
		return;

	RenderBackend_Blit(surf[from], &rcWork, surf[to], x * mag, y * mag, TRUE);
}

unsigned long GetCortBoxColor(unsigned long col)
{
	// Comes in 00BBGGRR, goes out 00BBGGRR
	return col;
}

void CortBox(const RECT *rect, unsigned long col)
{
	static RenderBackend_Rect rcSet;	// TODO - Not the original variable name
	rcSet.left = rect->left * mag;
	rcSet.top = rect->top * mag;
	rcSet.right = rect->right * mag;
	rcSet.bottom = rect->bottom * mag;

	const unsigned char red = col & 0xFF;
	const unsigned char green = (col >> 8) & 0xFF;
	const unsigned char blue = (col >> 16) & 0xFF;

	// Do not draw invalid RECTs
	if (rcSet.right <= rcSet.left || rcSet.bottom <= rcSet.top)
		return;

	RenderBackend_ColourFill(framebuffer, &rcSet, red, green, blue);
}

void CortBox2(const RECT *rect, unsigned long col, SurfaceID surf_no)
{
	if (surf[surf_no] == NULL)
		return;

	static RenderBackend_Rect rcSet;	// TODO - Not the original variable name
	rcSet.left = rect->left * mag;
	rcSet.top = rect->top * mag;
	rcSet.right = rect->right * mag;
	rcSet.bottom = rect->bottom * mag;

	surface_metadata[surf_no].type = SURFACE_SOURCE_NONE;

	const unsigned char red = col & 0xFF;
	const unsigned char green = (col >> 8) & 0xFF;
	const unsigned char blue = (col >> 16) & 0xFF;

	// Do not draw invalid RECTs
	if (rcSet.right <= rcSet.left || rcSet.bottom <= rcSet.top)
		return;

	RenderBackend_ColourFill(surf[surf_no], &rcSet, red, green, blue);
}

// Dummied-out log function
// According to the Mac port, its name really is just "out".
BOOL out(char surface_identifier)
{
	// The actual name (and type) of these two variables are unknown
	std::string path;
	FILE *fp;

	(void)surface_identifier;
	(void)path;
	(void)fp;

	// There may have been some kind of 'OutputDebugStringA' call here,
	// like the one in 'EnumDevices_Callback' in 'Input.cpp'.
	// Pixel may have kept them wrapped in '#ifdef DEBUG' blocks.

	return TRUE;
}

// TODO - Probably not the original function name (this is an educated guess)
int RestoreSurfaces(void)
{
	int s;
	RECT rect;
	int surfaces_regenerated = 0;

	if (framebuffer == NULL)
		return surfaces_regenerated;

	if (RenderBackend_IsSurfaceLost(framebuffer))
	{
		++surfaces_regenerated;
		RenderBackend_RestoreSurface(framebuffer);
		out('f');	// 'f' for 'frontbuffer' (or, in this branch's case, 'framebuffer')
	}

	for (s = 0; s < SURFACE_ID_MAX; ++s)
	{
		if (surf[s] != NULL)
		{
			if (RenderBackend_IsSurfaceLost(surf[s]))
			{
				++surfaces_regenerated;
				RenderBackend_RestoreSurface(surf[s]);
				out('0' + s);	// The number of the surface lost

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

#ifdef FREETYPE_FONTS
	std::string path = gDataPath + "/Font/font";

	// Get font size
	size_t width, height;

	// Let me tell you why these font sizes are unfortunate...
	// 6x12 is a good font size - fonts use high-quality bitmaps at that
	// size, and it works with Cave Story's internal assumption that
	// characters are spaced 6 pixels apart.
	// The sad part is the 10x20 size: you might be wondering why Pixel
	// didn't use 12x24 instead. Well, that's because fonts don't use
	// bitmaps at that size - instead you get ugly low-res vector
	// renders. So, Pixel had to use 10x20 instead. But there's a
	// problem: this means the characters are spaced 5 pixels apart
	// instead. This normally isn't a problem because the game usually
	// hardcodes it, but this isn't the case when either <SAT is used, a
	// texture is regenerated, or when the game prints the name of the
	// map. This causes the font to render with incorrect spacing.
	// There's really not much of a solution to this, especially when
	// you consider that the English translation was specifically built
	// around the broken spacing.

	switch (mag)
	{
		case 1:
		#ifdef JAPANESE
			height = 12;
			width = 6;
		#else
			// For some weird reason, Windows's 6x12 Courier New is
			// closer to 5x10, but FreeType renders it as proper 6x12,
			// so we have to cheat a little.
			height = 10;
			width = 5;
		#endif
			break;

		case 2:
			height = 20;
			width = 10;
			break;
	}

	font = LoadFreeTypeFont(path.c_str(), width, height);
#else
	std::string bitmap_path;
	std::string metadata_path;

	switch (mag)
	{
		case 1:
			bitmap_path = gDataPath + "/Font/font_bitmap_6x12.png";
			metadata_path = gDataPath + "/Font/font_bitmap_6x12.dat";
			break;

		case 2:
			bitmap_path = gDataPath + "/Font/font_bitmap_10x20.png";
			metadata_path = gDataPath + "/Font/font_bitmap_10x20.dat";
			break;
	}

	font = LoadBitmapFont(bitmap_path.c_str(), metadata_path.c_str());
#endif
}

void PutText(int x, int y, const char *text, unsigned long color)
{
	DrawText(font, framebuffer, x * mag, y * mag, color, text);
}

void PutText2(int x, int y, const char *text, unsigned long color, SurfaceID surf_no)
{
	if (surf[surf_no] == NULL)
		return;

	DrawText(font, surf[surf_no], x * mag, y * mag, color, text);
}

void EndTextObject(void)
{
	UnloadFont(font);
}
