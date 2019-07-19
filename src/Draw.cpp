#include <stddef.h>
#include <stdio.h>
#ifdef WINDOWS
#include <stdlib.h>
#endif
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Font.h"
#include "Resource.h"
#include "Tags.h"
#include "Backends/Rendering.h"

struct SURFACE
{
	BOOL in_use;
	Backend_Surface *backend;
};

SDL_Window *gWindow;

static SDL_PixelFormat *rgb24_pixel_format;	// Needed because SDL2 is stupid

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

int magnification;
BOOL fullscreen;

SURFACE surf[SURFACE_ID_MAX];

FontObject *gFont;

#define FRAMERATE 20

BOOL Flip_SystemTask(HWND hWnd)
{
	(void)hWnd;

	while (TRUE)
	{
		if (!SystemTask())
			return FALSE;

		// Framerate limiter
		static Uint32 timePrev;
		const Uint32 timeNow = SDL_GetTicks();

		if (timeNow >= timePrev + FRAMERATE)
		{
			if (timeNow >= timePrev + 100)
				timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
			else
				timePrev += FRAMERATE;

			break;
		}

		SDL_Delay(1);
	}

	Backend_DrawScreen();

	return TRUE;
}

BOOL StartDirectDraw(int lMagnification, int lColourDepth)
{
	(void)lColourDepth;	// There's no way I'm supporting a bunch of different colour depths

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
			SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
			break;
	}

	rgb24_pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);

	// Create renderer
	if (!Backend_Init(gWindow))
		return FALSE;

	return TRUE;
}

void EndDirectDraw()
{
	// Release all surfaces
	for (int i = 0; i < SURFACE_ID_MAX; i++)
		ReleaseSurface(i);

	Backend_Deinit();

	SDL_FreeFormat(rgb24_pixel_format);
}

static BOOL IsEnableBitmap(SDL_RWops *fp)
{
	char str[16];
	const char *extra_text = "(C)Pixel";

	const size_t len = strlen(extra_text);

	fp->seek(fp, -(Sint64)len, RW_SEEK_END);
	fp->read(fp, str, 1, len);
	fp->seek(fp, 0, RW_SEEK_SET);
	return memcmp(str, extra_text, len) == 0;
}

void ReleaseSurface(int s)
{
	// Release the surface we want to release
	if (surf[s].in_use)
	{
		Backend_FreeSurface(surf[s].backend);
		surf[s].in_use = FALSE;
	}
}

BOOL MakeSurface_Generic(int bxsize, int bysize, Surface_Ids surf_no, BOOL bSystem)
{
	(void)bSystem;

	BOOL success = FALSE;

#ifdef FIX_BUGS
	if (surf_no >= SURFACE_ID_MAX)
#else
	if (surf_no > SURFACE_ID_MAX)	// OOPS (should be '>=')
#endif
	{
		printf("Tried to create drawable surface at invalid slot (%d - maximum is %d)\n", surf_no, SURFACE_ID_MAX);
	}
	else
	{
		if (surf[surf_no].in_use)
		{
			printf("Tried to create drawable surface at occupied slot (%d)\n", surf_no);
		}
		else
		{
			// Create surface
			surf[surf_no].backend = Backend_CreateSurface(bxsize * magnification, bysize * magnification);

			if (surf[surf_no].backend == NULL)
			{
				printf("Failed to create backend surface %d\n", surf_no);
			}
			else
			{
				surf[surf_no].in_use = TRUE;
				success = TRUE;
			}
		}
	}

	return success;
}

static BOOL LoadBitmap(SDL_RWops *fp, Surface_Ids surf_no, BOOL create_surface)
{
	BOOL success = FALSE;

	if (surf_no >= SURFACE_ID_MAX)
	{
		printf("Tried to load bitmap at invalid slot (%d - maximum is %d\n", surf_no, SURFACE_ID_MAX);
	}
	else
	{
		if (create_surface && surf[surf_no].in_use)
		{
			printf("Tried to create drawable surface at occupied slot (%d)\n", surf_no);
		}
		else
		{
			SDL_Surface *surface = SDL_LoadBMP_RW(fp, 0);

			if (surface == NULL)
			{
				printf("Couldn't load bitmap for surface id %d\nSDL Error: %s\n", surf_no, SDL_GetError());
			}
			else
			{
				if (create_surface == FALSE || MakeSurface_Generic(surface->w, surface->h, surf_no, FALSE))
				{

					SDL_Surface *converted_surface = SDL_ConvertSurface(surface, rgb24_pixel_format, 0);

					if (converted_surface == NULL)
					{
						printf("Couldn't convert bitmap to surface format (surface id %d)\nSDL Error: %s\n", surf_no, SDL_GetError());
					}
					else
					{
						// IF YOU WANT TO ADD HD SPRITES, THIS IS THE CODE YOU SHOULD EDIT
						unsigned int pitch;
						unsigned char *pixels = Backend_Lock(surf[surf_no].backend, &pitch);

						if (magnification == 1)
						{
							// Just copy the pixels the way they are
							for (int y = 0; y < converted_surface->h; ++y)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + y * converted_surface->pitch;
								unsigned char *dst_row = &pixels[y * pitch];

								memcpy(dst_row, src_row, converted_surface->w * 3);
							}
						}
						else
						{
							// Upscale the bitmap to the game's internal resolution
							for (int y = 0; y < converted_surface->h; ++y)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + y * converted_surface->pitch;
								unsigned char *dst_row = &pixels[y * pitch * magnification];

								const unsigned char *src_ptr = src_row;
								unsigned char *dst_ptr = dst_row;

								for (int x = 0; x < converted_surface->w; ++x)
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
									memcpy(dst_row + i * pitch, dst_row, converted_surface->w * magnification * 3);
							}
						}

						Backend_Unlock(surf[surf_no].backend);
						SDL_FreeSurface(converted_surface);
						printf(" ^ Successfully loaded\n");
						success = TRUE;
					}
				}

				SDL_FreeSurface(surface);
			}
		}
	}

	fp->close(fp);

	return success;
}

static BOOL LoadBitmap_File(const char *name, Surface_Ids surf_no, BOOL create_surface)
{
	char path[PATH_LENGTH];
	SDL_RWops *fp;

	// Attempt to load PBM
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

	// Attempt to load BMP
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
}

static BOOL LoadBitmap_Resource(const char *res, Surface_Ids surf_no, BOOL create_surface)
{
	size_t size;
	const unsigned char *data = FindResource(res, "BITMAP", &size);

	if (data)
	{
		// For some dumbass reason, SDL2 measures size with a signed int.
		// Has anyone ever told the devs that an int can be as little as 16 bits long? Real portable.
		// But hey, if I ever need to create an RWops from an array that's -32768 bytes long, they've got me covered!
		SDL_RWops *fp = SDL_RWFromConstMem(data, size);

		printf("Loading surface from resource %s for surface id %d\n", res, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return TRUE;
	}

	printf("Failed to open resource %s\n", res);
	return FALSE;
}

BOOL MakeSurface_File(const char *name, Surface_Ids surf_no)
{
	return LoadBitmap_File(name, surf_no, TRUE);
}

BOOL MakeSurface_Resource(const char *res, Surface_Ids surf_no)
{
	return LoadBitmap_Resource(res, surf_no, TRUE);
}

BOOL ReloadBitmap_File(const char *name, Surface_Ids surf_no)
{
	return LoadBitmap_File(name, surf_no, FALSE);
}

BOOL ReloadBitmap_Resource(const char *res, Surface_Ids surf_no)
{
	return LoadBitmap_Resource(res, surf_no, FALSE);
}

static void ScaleRect(const RECT *source_rect, RECT *destination_rect)
{
	destination_rect->left = source_rect->left * magnification;
	destination_rect->top = source_rect->top * magnification;
	destination_rect->right = source_rect->right * magnification;
	destination_rect->bottom = source_rect->bottom * magnification;
}

void BackupSurface(Surface_Ids surf_no, const RECT *rect)
{
	RECT frameRect;
	ScaleRect(rect, &frameRect);

	Backend_ScreenToSurface(surf[surf_no].backend, &frameRect);
}

static void DrawBitmap(const RECT *rcView, int x, int y, const RECT *rect, Surface_Ids surf_no, BOOL transparent)
{
	RECT frameRect;

	frameRect.left = rect->left;
	frameRect.top = rect->top;
	frameRect.right = rect->right;
	frameRect.bottom = rect->bottom;

	if (x + (rect->right - rect->left) > rcView->right)
	{
		frameRect.right -= (x + (rect->right - rect->left)) - rcView->right;
	}

	if (x < rcView->left)
	{
		frameRect.left += rcView->left - x;
		x = rcView->left;
	}

	if (y + (rect->bottom - rect->top) > rcView->bottom)
	{
		frameRect.bottom -= (y + (rect->bottom - rect->top)) - rcView->bottom;
	}

	if (y < rcView->top)
	{
		frameRect.top += rcView->top - y;
		y = rcView->top;
	}

	frameRect.left *= magnification;
	frameRect.top *= magnification;
	frameRect.right *= magnification;
	frameRect.bottom *= magnification;

	// Draw to screen
	Backend_BlitToScreen(surf[surf_no].backend, &frameRect, x * magnification, y * magnification, transparent);
}

void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, Surface_Ids surf_no) // Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, TRUE);
}

void PutBitmap4(const RECT *rcView, int x, int y, const RECT *rect, Surface_Ids surf_no) // No Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, FALSE);
}

void Surface2Surface(int x, int y, const RECT *rect, int to, int from)
{
	// Get rects
	RECT frameRect;
	ScaleRect(rect, &frameRect);

	Backend_Blit(surf[from].backend, &frameRect, surf[to].backend, x * magnification, y * magnification, TRUE);
}

unsigned long GetCortBoxColor(unsigned long col)
{
	// In vanilla, this called a DirectDraw function to convert it to the 'native' colour type.
	// Here, we just return the colour in its original BGR form.
	return col;
}

void CortBox(const RECT *rect, unsigned long col)
{
	// Get rect
	RECT destRect;
	ScaleRect(rect, &destRect);

	// Set colour and draw
	const unsigned char col_red = (unsigned char)(col & 0xFF);
	const unsigned char col_green = (unsigned char)((col >> 8) & 0xFF);
	const unsigned char col_blue = (unsigned char)((col >> 16) & 0xFF);

	Backend_ColourFillToScreen(&destRect, col_red, col_green, col_blue);
}

void CortBox2(const RECT *rect, unsigned long col, Surface_Ids surf_no)
{
	// Get rect
	RECT destRect;
	ScaleRect(rect, &destRect);

	// Set colour and draw
	const unsigned char col_red = (unsigned char)(col & 0xFF);
	const unsigned char col_green = (unsigned char)((col >> 8) & 0xFF);
	const unsigned char col_blue = (unsigned char)((col >> 16) & 0xFF);

	Backend_ColourFill(surf[surf_no].backend, &destRect, col_red, col_green, col_blue);
}

#ifdef WINDOWS
static unsigned char* GetFontFromWindows(size_t *data_size, const char *font_name, unsigned int fontWidth, unsigned int fontHeight)
{
	unsigned char* buffer = NULL;

#ifdef JAPANESE
	const DWORD charset = SHIFTJIS_CHARSET;
#else
	const DWORD charset = DEFAULT_CHARSET;
#endif

	HFONT hfont = CreateFontA(fontHeight, fontWidth, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, charset, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, font_name);

	if (hfont != NULL)
	{
		HDC hdc = CreateCompatibleDC(NULL);

		if (hdc != NULL)
		{
			SelectObject(hdc, hfont);
			const DWORD size = GetFontData(hdc, 0, 0, NULL, 0);

			if (size != GDI_ERROR)
			{
				buffer = (unsigned char*)malloc(size);

				if (data_size != NULL)
					*data_size = size;

				if (GetFontData(hdc, 0, 0, buffer, size) != size)
				{
					free(buffer);
					buffer = NULL;
				}
			}

			DeleteDC(hdc);
		}
	}

	return buffer;
}
#endif

void InitTextObject(const char *font_name)
{
	// Get font size
	unsigned int fontWidth, fontHeight;

	// The original did this, but Windows would downscale it to 5/10 anyway.
/*	if (magnification == 1)
	{
		fontWidth = 6;
		fontHeight = 12;
	}
	else
	{
		fontWidth = 5 * magnification;
		fontHeight = 10 * magnification;
	}*/

	fontWidth = 5 * magnification;
	fontHeight = 10 * magnification;

	size_t data_size;
#ifdef WINDOWS
	// Actually use the font Config.dat specifies
	unsigned char *data;
	data = GetFontFromWindows(&data_size, font_name, fontWidth, fontHeight);
	if (data)
	{
		gFont = LoadFontFromData(data, data_size, fontWidth, fontHeight);
		free(data);
	}

	if (gFont)
		return;

#ifndef JAPANESE
	// Fall back on a default font
	data = GetFontFromWindows(&data_size, "Courier New", fontWidth, fontHeight);
	if (data)
	{
		gFont = LoadFontFromData(data, data_size, fontWidth, fontHeight);
		free(data);
	}

	if (gFont)
		return;
#endif
#endif
	// Fall back on the built-in font
	(void)font_name;
	const unsigned char *res_data = FindResource("DEFAULT_FONT", "FONT", &data_size);

	if (res_data != NULL)
		gFont = LoadFontFromData(res_data, data_size, fontWidth, fontHeight);
}

void PutText(int x, int y, const char *text, unsigned long color)
{
	Backend_DrawTextToScreen(gFont, x * magnification, y * magnification, text, color);
}

void PutText2(int x, int y, const char *text, unsigned long color, Surface_Ids surf_no)
{
	Backend_DrawText(surf[surf_no].backend, gFont, x * magnification, y * magnification, text, color);
}

void EndTextObject()
{
	// Destroy font
	UnloadFont(gFont);
	gFont = NULL;
}

void HandleDeviceLoss()
{
	Backend_HandleDeviceLoss();
}

void HandleWindowResize()
{
	Backend_HandleWindowResize();
}
