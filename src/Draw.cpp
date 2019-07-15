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

struct SURFACE
{
	BOOL in_use;
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
};

SDL_Window *gWindow;
static SDL_Surface *gWindowSurface;
static SDL_Surface *gSurface;

static SURFACE framebuffer;

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

	SDL_BlitSurface(gSurface, NULL, gWindowSurface, NULL);
	SDL_UpdateWindowSurface(gWindow);

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

	// Create renderer
	gWindowSurface = SDL_GetWindowSurface(gWindow);

	gSurface = SDL_CreateRGBSurfaceWithFormat(0, WINDOW_WIDTH * magnification, WINDOW_HEIGHT * magnification, 0, SDL_PIXELFORMAT_RGB24);

	if (gSurface == NULL)
		return FALSE;

	framebuffer.in_use = TRUE;
	framebuffer.pixels = (unsigned char*)gSurface->pixels;
	framebuffer.width = WINDOW_WIDTH * magnification;
	framebuffer.height = WINDOW_HEIGHT * magnification;
	framebuffer.pitch = gSurface->pitch;

	return TRUE;
}

void EndDirectDraw()
{
	// Release all surfaces
	for (int i = 0; i < SURFACE_ID_MAX; i++)
		ReleaseSurface(i);

	framebuffer.in_use = FALSE;

	SDL_FreeSurface(gSurface);
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
		free(surf[s].pixels);
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
			surf[surf_no].pixels = (unsigned char*)malloc((bxsize * magnification) * (bysize * magnification) * 3);
			surf[surf_no].width = bxsize * magnification;
			surf[surf_no].height = bysize * magnification;
			surf[surf_no].pitch = surf[surf_no].width * 3;

			if (surf[surf_no].pixels == NULL)
			{
				printf("Failed to allocate surface pixel buffer %d\n", surf_no);
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

					SDL_Surface *converted_surface = SDL_ConvertSurface(surface, gSurface->format, 0);

					if (converted_surface == NULL)
					{
						printf("Couldn't convert bitmap to surface format (surface id %d)\nSDL Error: %s\n", surf_no, SDL_GetError());
					}
					else
					{
						// IF YOU WANT TO ADD HD SPRITES, THIS IS THE CODE YOU SHOULD EDIT
						if (magnification == 1)
						{
							// Just copy the pixels the way they are
							for (int h = 0; h < converted_surface->h; ++h)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + h * converted_surface->pitch;
								unsigned char *dst_row = surf[surf_no].pixels + h * surf[surf_no].pitch;

								memcpy(dst_row, src_row, converted_surface->w * 3);
							}
						}
						else
						{
							// Upscale the bitmap to the game's internal resolution
							for (int h = 0; h < converted_surface->h; ++h)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + h * converted_surface->pitch;
								unsigned char *dst_row = surf[surf_no].pixels + h * surf[surf_no].pitch * magnification;

								const unsigned char *src_ptr = src_row;
								unsigned char *dst_ptr = dst_row;

								for (int w = 0; w < converted_surface->w; ++w)
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
									memcpy(dst_row + i * surf[surf_no].pitch, dst_row, converted_surface->w * magnification * 3);
							}
						}

						SDL_FreeSurface(converted_surface);
						//surf[surf_no].needs_updating = TRUE;
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
/*
static SDL_Rect RectToSDLRect(RECT *rect)
{
	SDL_Rect SDLRect = {(int)rect->left, (int)rect->top, (int)(rect->right - rect->left), (int)(rect->bottom - rect->top)};
	if (SDLRect.w < 0)
		SDLRect.w = 0;
	if (SDLRect.h < 0)
		SDLRect.h = 0;
	return SDLRect;
}

static SDL_Rect RectToSDLRectScaled(RECT *rect)
{
	SDL_Rect SDLRect = RectToSDLRect(rect);
	SDLRect.x *= magnification;
	SDLRect.y *= magnification;
	SDLRect.w *= magnification;
	SDLRect.h *= magnification;
	return SDLRect;
}
*/

static void ScaleRect(const RECT *source_rect, RECT *destination_rect)
{
	destination_rect->left = source_rect->left * magnification;
	destination_rect->top = source_rect->top * magnification;
	destination_rect->right = source_rect->right * magnification;
	destination_rect->bottom = source_rect->bottom * magnification;
}

static void Blit(const SURFACE *source_surface, const RECT *rect, SURFACE *destination_surface, long x, long y, BOOL colour_key)
{
	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect and coordinates so we don't write outside the pixel buffer
	long overflow;

	overflow = 0 - x;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
		x += overflow;
	}

	overflow = 0 - y;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
		y += overflow;
	}

	overflow = (x + (rect_clamped.right - rect_clamped.left)) - destination_surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = (y + (rect_clamped.bottom - rect_clamped.top)) - destination_surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	// Do the actual blitting
	if (colour_key)
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 3)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 3)];

			for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
			{
				if (source_pointer[0] != 0 || source_pointer[1] != 0 || source_pointer[2] != 0)	// Assumes the colour key will always be #00000000 (black)
				{
					destination_pointer[0] = source_pointer[0];
					destination_pointer[1] = source_pointer[1];
					destination_pointer[2] = source_pointer[2];
				}

				source_pointer += 3;
				destination_pointer += 3;
			}
		}
	}
	else
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 3)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 3)];

			memcpy(destination_pointer, source_pointer, (rect_clamped.right - rect_clamped.left) * 3);
		}
	}
}

static void ColourFill(SURFACE *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect so it doesn't write outside the pixel buffer
	long overflow;

	overflow = 0 - rect_clamped.left;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
	}

	overflow = 0 - rect_clamped.top;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
	}

	overflow = rect_clamped.right - surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = rect_clamped.bottom - surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
	{
		unsigned char *source_pointer = &surface->pixels[((rect_clamped.top + j) * surface->pitch) + (rect_clamped.left * 3)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			*source_pointer++ = red;
			*source_pointer++ = green;
			*source_pointer++ = blue;
		}
	}
}

void BackupSurface(Surface_Ids surf_no, const RECT *rect)
{
	RECT frameRect;
	ScaleRect(rect, &frameRect);

	Blit(&framebuffer, &frameRect, &surf[surf_no], frameRect.left, frameRect.top, FALSE);
	//surf[surf_no].needs_updating = TRUE;
}

static void DrawBitmap(const RECT *rcView, int x, int y, const RECT *rect, Surface_Ids surf_no, BOOL transparent)
{
/*	if (surf[surf_no].needs_updating)
	{
		FlushSurface(surf_no);
		surf[surf_no].needs_updating = FALSE;
	}
*/
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
	Blit(&surf[surf_no], &frameRect, &framebuffer, x * magnification, y * magnification, transparent);
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

	Blit(&surf[from], &frameRect, &surf[to], x * magnification, y * magnification, TRUE);
	//surf[to].needs_updating = TRUE;
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

	ColourFill(&framebuffer, &destRect, col_red, col_green, col_blue);
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

	ColourFill(&surf[surf_no], &destRect, col_red, col_green, col_blue);
	//surf[surf_no].needs_updating = TRUE;
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
	DrawText(gFont, framebuffer.pixels, framebuffer.pitch, framebuffer.width, framebuffer.height, x * magnification, y * magnification, color, text, strlen(text));
}

void PutText2(int x, int y, const char *text, unsigned long color, Surface_Ids surf_no)
{
	DrawText(gFont, surf[surf_no].pixels, surf[surf_no].pitch, surf[surf_no].width, surf[surf_no].height, x * magnification, y * magnification, color, text, strlen(text));
	//surf[surf_no].needs_updating = TRUE;
}

void EndTextObject()
{
	// Destroy font
	UnloadFont(gFont);
	gFont = NULL;
}
