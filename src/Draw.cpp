#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "lodepng/lodepng.h"

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
static SDL_Renderer *gRenderer;
static SDL_Texture *screen_texture;

static SDL_PixelFormat *rgba32_pixel_format;	// Needed because SDL2 is stupid

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

int magnification;
BOOL fullscreen;

SURFACE surf[SURFACE_ID_MAX];

FontObject *gFont;

static BOOL vsync;

BOOL Flip_SystemTask(HWND hWnd)
{
	(void)hWnd;

	if (vsync)
	{
		if (!SystemTask())
			return FALSE;
	}
	else
	{
		while (TRUE)
		{
			const unsigned int frameDelays[3] = {17, 16, 17};
			static unsigned int frame;

			if (!SystemTask())
				return FALSE;

			// Framerate limiter
			static Uint32 timePrev;
			const Uint32 timeNow = SDL_GetTicks();

			if (timeNow >= timePrev + frameDelays[frame % 3])
			{
				if (timeNow >= timePrev + 100)
					timePrev = timeNow;	// If the timer is freakishly out of sync, panic and reset it, instead of spamming frames for who-knows how long
				else
					timePrev += frameDelays[frame++ % 3];

				break;
			}

			SDL_Delay(1);
		}
	}

	Backend_DrawScreen();

	SDL_SetRenderTarget(gRenderer, NULL);

	int renderer_width, renderer_height;
	SDL_GetRendererOutputSize(gRenderer, &renderer_width, &renderer_height);

	int texture_width, texture_height;
	SDL_QueryTexture(screen_texture, NULL, NULL, &texture_width, &texture_height);

	SDL_Rect dst_rect;
	if ((float)renderer_width / texture_width < (float)renderer_height / texture_height)
	{
		dst_rect.w = renderer_width;
		dst_rect.h = (int)(texture_height * (float)renderer_width / texture_width);
		dst_rect.x = 0;
		dst_rect.y = (renderer_height - dst_rect.h) / 2;
	}
	else
	{
		dst_rect.w = (int)(texture_width * (float)renderer_height / texture_height);
		dst_rect.h = renderer_height;
		dst_rect.x = (renderer_width - dst_rect.w) / 2;
		dst_rect.y = 0;
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, screen_texture, NULL, &dst_rect);

	SDL_SetRenderTarget(gRenderer, screen_texture);

	SDL_RenderPresent(gRenderer);

	return TRUE;
}

BOOL StartDirectDraw(int lMagnification)
{
	switch (lMagnification)
	{
		default:
			magnification = lMagnification;
			fullscreen = FALSE;
			break;

		case 0:
			SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			int width, height;
			SDL_GetWindowSize(gWindow, &width, &height);
			magnification = width / WINDOW_WIDTH < height / WINDOW_HEIGHT ? width / WINDOW_WIDTH : height / WINDOW_HEIGHT;
			fullscreen = TRUE;
			break;
	}

	// Ugly way to round the magnification up to the nearest multiple of SPRITE_SCALE (we can't use 2x sprites at 1x or 3x internal resolution)
	magnification = ((magnification + (SPRITE_SCALE - 1)) / SPRITE_SCALE) * SPRITE_SCALE;

	// Check if vsync is possible
	SDL_DisplayMode display_mode;
	SDL_GetWindowDisplayMode(gWindow, &display_mode);
	vsync = display_mode.refresh_rate == 60;

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

	if (gRenderer == NULL)
		return FALSE;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	screen_texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH * magnification, WINDOW_HEIGHT * magnification);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_SetRenderTarget(gRenderer, screen_texture);

	rgba32_pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

	// Create renderer
	if (!Backend_Init(gRenderer))
		return FALSE;

	return TRUE;
}

void EndDirectDraw()
{
	// Release all surfaces
	for (int i = 0; i < SURFACE_ID_MAX; i++)
		ReleaseSurface(i);

	Backend_Deinit();

	SDL_FreeFormat(rgba32_pixel_format);

	SDL_DestroyTexture(screen_texture);
	SDL_DestroyRenderer(gRenderer);
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
		printf("Tried to load bitmap at invalid slot (%d - maximum is %d)\n", surf_no, SURFACE_ID_MAX);
	}
	else
	{
		if (create_surface && surf[surf_no].in_use)
		{
			printf("Tried to create drawable surface at occupied slot (%d)\n", surf_no);
		}
		else
		{
			char fourcc[2];
			fp->read(fp, fourcc, 1, 2);
			fp->seek(fp, 0, RW_SEEK_SET);

			BOOL is_bmp = (fourcc[0] == 'B' && fourcc[1] == 'M');

			SDL_Surface *surface;

			if (is_bmp)
			{
				// BMP file
				surface = SDL_LoadBMP_RW(fp, 0);
				SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
			}
			else
			{
				// PNG file
				const size_t file_size = (size_t)fp->size(fp);
				unsigned char *file_buffer = (unsigned char*)malloc(file_size);
				fp->read(fp, file_buffer, 1, file_size);

				unsigned int bitmap_width, bitmap_height;
				unsigned char *bitmap_pixels;
				lodepng_decode32(&bitmap_pixels, &bitmap_width, &bitmap_height, file_buffer, file_size);
				free(file_buffer);

				surface = SDL_CreateRGBSurfaceWithFormatFrom(bitmap_pixels, bitmap_width, bitmap_height, 0, bitmap_width * 4, SDL_PIXELFORMAT_RGBA32);
			}

			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

			if (surface == NULL)
			{
				printf("Couldn't load bitmap for surface id %d\nSDL Error: %s\n", surf_no, SDL_GetError());
			}
			else
			{
				if (create_surface == FALSE || MakeSurface_Generic(surface->w, surface->h, surf_no, FALSE))
				{

					SDL_Surface *converted_surface = SDL_ConvertSurface(surface, rgba32_pixel_format, 0);

					if (converted_surface == NULL)
					{
						printf("Couldn't convert bitmap to surface format (surface id %d)\nSDL Error: %s\n", surf_no, SDL_GetError());
					}
					else
					{
						unsigned int pitch;
						unsigned char *pixels = Backend_Lock(surf[surf_no].backend, &pitch);

						const int magnification_scaled = magnification / SPRITE_SCALE;

						if (magnification_scaled == 1)
						{
							// Just copy the pixels the way they are
							for (int y = 0; y < converted_surface->h; ++y)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + y * converted_surface->pitch;
								unsigned char *dst_row = &pixels[y * pitch];

								memcpy(dst_row, src_row, converted_surface->w * 4);
							}
						}
						else
						{
							// Upscale the bitmap to the game's internal resolution
							for (int y = 0; y < converted_surface->h; ++y)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + y * converted_surface->pitch;
								unsigned char *dst_row = &pixels[y * pitch * magnification_scaled];

								const unsigned char *src_ptr = src_row;
								unsigned char *dst_ptr = dst_row;

								for (int x = 0; x < converted_surface->w; ++x)
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
									memcpy(dst_row + i * pitch, dst_row, converted_surface->w * magnification_scaled * 4);
							}
						}

						Backend_Unlock(surf[surf_no].backend);
						SDL_FreeSurface(converted_surface);
						printf(" ^ Successfully loaded\n");
						success = TRUE;
					}
				}

				if (!is_bmp)
					free(surface->pixels);

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
		printf("Loading surface (as .pbm) from %s for surface id %d\n", path, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return TRUE;
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

	// Attempt to load PNG
	sprintf(path, "%s/%s.png", gDataPath, name);
	fp = SDL_RWFromFile(path, "rb");
	if (fp)
	{
		printf("Loading surface (as .png) from %s for surface id %d\n", path, surf_no);
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
	ScaleRect(rect, &frameRect);

	RECT clipRect;
	ScaleRect(rcView, &clipRect);

	if (x + (frameRect.right - frameRect.left) > clipRect.right)
	{
		frameRect.right -= (x + (frameRect.right - frameRect.left)) - clipRect.right;
	}

	if (x < clipRect.left)
	{
		frameRect.left += clipRect.left - x;
		x = clipRect.left;
	}

	if (y + (frameRect.bottom - frameRect.top) > clipRect.bottom)
	{
		frameRect.bottom -= (y + (frameRect.bottom - frameRect.top)) - clipRect.bottom;
	}

	if (y < clipRect.top)
	{
		frameRect.top += clipRect.top - y;
		y = clipRect.top;
	}

	// Draw to screen
	Backend_BlitToScreen(surf[surf_no].backend, &frameRect, x, y, transparent);
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

	// Note for future-me: this has to ignore alpha because you obviously
	// can't have transparent parts of the window. It'll just appear as
	// solid black. Setting the alpha as anything but 255 in this case
	// can break things, like the 'Studio Pixel presents' text rendering
	// in the software renderer, since it blends using the alpha value
	// that doesn't do anything.
	// Besides, the original game only used colour-keying on surface blits,
	// not the drawing of the actual window.

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
	const unsigned char col_alpha = (unsigned char)((col >> 24) & 0xFF);

	Backend_ColourFill(surf[surf_no].backend, &destRect, col_red, col_green, col_blue, col_alpha);
}

int SubpixelToScreenCoord(int coord)
{
#ifdef SMOOTH_SPRITE_MOVEMENT
	return (coord * magnification) / 0x200;
#else
	return (coord / (0x200 / SPRITE_SCALE)) * (magnification / SPRITE_SCALE);
#endif
}

int PixelToScreenCoord(int coord)
{
	return coord * magnification;
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
