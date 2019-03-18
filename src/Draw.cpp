#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef WINDOWS
#define RECT WINRECT
#define FindResource WinFindResource	// All these damn name collisions...
#define DrawText WinDrawText
#define LoadFont WinLoadFont
#include <windows.h>
#undef LoadFont
#undef DrawText
#undef FindResource
#undef RECT
#endif

#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_timer.h>

#include "lodepng/lodepng.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Font.h"
#include "Resource.h"
#include "Tags.h"
#include "Types.h"

struct SURFACE
{
	bool in_use;
	bool needs_updating;
	SDL_Surface *surface;
	SDL_Texture *texture;
};

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

static SDL_Texture *native_res_render_target;
static bool vsync;

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

int magnification;
bool fullscreen;

SURFACE surf[SURFACE_ID_MAX];

FontObject *gFont;

BOOL Flip_SystemTask()
{
	if (vsync)
	{
		if (!SystemTask())
			return FALSE;
	}
	else
	{
		while (true)
		{
			const unsigned int frameDelays[3] = {17, 16, 17};
			static unsigned int frame;

			if (!SystemTask())
				return FALSE;

			//Framerate limiter
			static uint32_t timePrev;
			const uint32_t timeNow = SDL_GetTicks();

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

	SDL_SetRenderTarget(gRenderer, NULL);

	int renderer_width, renderer_height;
	SDL_GetRendererOutputSize(gRenderer, &renderer_width, &renderer_height);

	int texture_width, texture_height;
	SDL_QueryTexture(native_res_render_target, NULL, NULL, &texture_width, &texture_height);

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
	SDL_RenderCopy(gRenderer, native_res_render_target, NULL, &dst_rect);

	SDL_SetRenderTarget(gRenderer, native_res_render_target);

	SDL_RenderPresent(gRenderer);
	return TRUE;
}

BOOL StartDirectDraw(int lMagnification, int lColourDepth)
{
	(void)lColourDepth;

	//Initialize rendering
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	//Check if vsync is possible
	SDL_DisplayMode display_mode;
	SDL_GetWindowDisplayMode(gWindow, &display_mode);
	vsync = display_mode.refresh_rate == 60;

#ifdef RASPBERRY_PI
	//Force OpenGLES2 on Raspberry Pi
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
#endif

	//Create renderer
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

	if (gRenderer != NULL)
	{
		//Print the name of the renderer SDL2 is using
		SDL_RendererInfo info;
		SDL_GetRendererInfo(gRenderer, &info);
		printf("Renderer: %s\n", info.name);

		switch (lMagnification)
		{
			case 0:
				magnification = 1;
				fullscreen = false;
				break;
				
			case 1:
				magnification = 2;
				fullscreen = false;
				break;
				
			case 2:
				SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
				int width, height;
				SDL_GetRendererOutputSize(gRenderer, &width, &height);
				magnification = width / WINDOW_WIDTH < height / WINDOW_HEIGHT ? width / WINDOW_WIDTH : height / WINDOW_HEIGHT;
				fullscreen = true;
				break;
		}

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		native_res_render_target = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH * magnification, WINDOW_HEIGHT * magnification);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
		SDL_SetRenderTarget(gRenderer, native_res_render_target);
	}
	
	return TRUE;
}

void EndDirectDraw()
{
	//Quit sub-system
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	
	//Release all surfaces
	for (int i = 0; i < SURFACE_ID_MAX; i++)
		ReleaseSurface(i);
}

void ReleaseSurface(int s)
{
	//Release the surface we want to release
	if (surf[s].in_use)
	{
		SDL_DestroyTexture(surf[s].texture);
		SDL_FreeSurface(surf[s].surface);
		surf[s].in_use = false;
	}
}

BOOL MakeSurface_Generic(int bxsize, int bysize, Surface_Ids surf_no)
{
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
		if (surf[surf_no].in_use == true)
		{
			printf("Tried to create drawable surface at occupied slot (%d)\n", surf_no);
		}
		else
		{
			//Create surface
			surf[surf_no].surface = SDL_CreateRGBSurfaceWithFormat(0, bxsize * magnification, bysize * magnification, 0, SDL_PIXELFORMAT_RGBA32);
			SDL_SetSurfaceBlendMode(surf[surf_no].surface, SDL_BLENDMODE_NONE);

			if (surf[surf_no].surface == NULL)
			{
				printf("Failed to create drawable surface %d (SDL_CreateRGBSurfaceWithFormat)\nSDL Error: %s\n", surf_no, SDL_GetError());
			}
			else
			{
				surf[surf_no].texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, bxsize * magnification, bysize * magnification);

				if (surf[surf_no].texture == NULL)
				{
					printf("Failed to create drawable surface %d (SDL_CreateTexture)\nSDL Error: %s\n", surf_no, SDL_GetError());
					SDL_FreeSurface(surf[surf_no].surface);
				}
				else
				{
					SDL_SetTextureBlendMode(surf[surf_no].texture, SDL_BLENDMODE_BLEND);
					surf[surf_no].in_use = true;
					success = TRUE;
				}
			}
		}
	}

	return success;
}

static void FlushSurface(Surface_Ids surf_no)
{
	unsigned char *raw_pixels;
	int pitch;
	SDL_LockTexture(surf[surf_no].texture, NULL, (void**)&raw_pixels, &pitch);

	for (int h = 0; h < surf[surf_no].surface->h; ++h)
		memcpy((unsigned char*)raw_pixels + (h * pitch), (unsigned char*)surf[surf_no].surface->pixels + (h * surf[surf_no].surface->pitch), surf[surf_no].surface->w * 4);

	SDL_UnlockTexture(surf[surf_no].texture);
}

static bool LoadBitmap(SDL_RWops *fp, Surface_Ids surf_no, bool create_surface)
{
	bool success = false;

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
			const size_t file_size = (size_t)fp->size(fp);
			unsigned char *file_buffer = (unsigned char*)malloc(file_size);
			fp->read(fp, file_buffer, 1, file_size);

			unsigned int bitmap_width, bitmap_height;
			unsigned char *bitmap_pixels;
			lodepng_decode32(&bitmap_pixels, &bitmap_width, &bitmap_height, file_buffer, file_size);
			free(file_buffer);

			SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(bitmap_pixels, bitmap_width, bitmap_height, 0, bitmap_width * 4, SDL_PIXELFORMAT_RGBA32);
			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

			if (surface == NULL)
			{
				printf("Couldn't load bitmap for surface id %d\nSDL Error: %s\n", surf_no, SDL_GetError());
			}
			else
			{
				if (create_surface == false || MakeSurface_Generic(surface->w, surface->h, surf_no))
				{
					if (magnification == 1)
					{
						SDL_Rect dst_rect = {0, 0, surface->w, surface->h};
						SDL_BlitSurface(surface, NULL, surf[surf_no].surface, &dst_rect);
						surf[surf_no].needs_updating = true;
						printf(" ^ Successfully loaded\n");
						success = true;
					}
					else
					{
						SDL_Surface *converted_surface = SDL_ConvertSurface(surface, surf[surf_no].surface->format, 0);

						if (converted_surface == NULL)
						{
							printf("Couldn't convert bitmap to surface format (surface id %d)\nSDL Error: %s\n", surf_no, SDL_GetError());
						}
						else
						{
							// Upscale the bitmap to the game's native resolution (SDL_BlitScaled is buggy, so we have to do it on our own)
							for (int h = 0; h < converted_surface->h; ++h)
							{
								const unsigned char *src_row = (unsigned char*)converted_surface->pixels + h * converted_surface->pitch;
								unsigned char *dst_row = (unsigned char*)surf[surf_no].surface->pixels + h * surf[surf_no].surface->pitch * magnification;

								const unsigned char *src_ptr = src_row;
								unsigned char *dst_ptr = dst_row;

								for (int w = 0; w < converted_surface->w; ++w)
								{
									for (int i = 0; i < magnification; ++i)
									{
										*dst_ptr++ = src_ptr[0];
										*dst_ptr++ = src_ptr[1];
										*dst_ptr++ = src_ptr[2];
										*dst_ptr++ = src_ptr[3];
									}

									src_ptr += 4;
								}

								for (int i = 1; i < magnification; ++i)
									memcpy(dst_row + i * surf[surf_no].surface->pitch, dst_row, surf[surf_no].surface->w * 4);
							}

							SDL_FreeSurface(converted_surface);
							surf[surf_no].needs_updating = true;
							printf(" ^ Successfully loaded\n");
							success = true;
						}
					}
				}

				SDL_FreeSurface(surface);
				free(bitmap_pixels);
			}
		}
	}

	fp->close(fp);

	return success;
}

static BOOL LoadBitmap_File(const char *name, Surface_Ids surf_no, bool create_surface)
{
	char path[PATH_LENGTH];
	SDL_RWops *fp;

	//Attempt to load PNG
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

static BOOL LoadBitmap_Resource(const char *res, Surface_Ids surf_no, bool create_surface)
{
	SDL_RWops *fp = FindResource(res);
	
	if (fp)
	{
		printf("Loading surface from resource %s for surface id %d\n", res, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return TRUE;
	}
	
	printf("Failed to open resource %s\n", res);
	return FALSE;
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

static SDL_Rect RectToSDLRect(RECT *rect)
{
	SDL_Rect SDLRect = {rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top};
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

void BackupSurface(Surface_Ids surf_no, RECT *rect)
{
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
}

static void DrawBitmap(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no)
{
	if (surf[surf_no].needs_updating)
	{
		FlushSurface(surf_no);
		surf[surf_no].needs_updating = false;
	}

	//Get SDL_Rects
	SDL_Rect clipRect = RectToSDLRectScaled(rcView);

	SDL_Rect frameRect = RectToSDLRectScaled(rect);
	
	//Get dest rect
	SDL_Rect destRect = {x, y, frameRect.w, frameRect.h};
	
	//Set cliprect
	SDL_RenderSetClipRect(gRenderer, &clipRect);
	
	//Draw to screen
	if (SDL_RenderCopy(gRenderer, surf[surf_no].texture, &frameRect, &destRect) < 0)
		printf("Failed to draw texture %d\nSDL Error: %s\n", surf_no, SDL_GetError());
	
	//Undo cliprect
	SDL_RenderSetClipRect(gRenderer, NULL);
}

void PutBitmap3(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no) //Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no);
}

void PutBitmap4(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no) //No Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no);
}

void Surface2Surface(int x, int y, RECT *rect, int to, int from)
{
	//Get rects
	SDL_Rect rcSet = {x * magnification, y * magnification, (rect->right - rect->left) * magnification, (rect->bottom - rect->top) * magnification};
	SDL_Rect frameRect = RectToSDLRectScaled(rect);

	SDL_BlitSurface(surf[from].surface, &frameRect, surf[to].surface, &rcSet);
	surf[to].needs_updating = true;
}

unsigned long GetCortBoxColor(unsigned long col)
{
	// This comes in BGR, and goes out BGR
	return col;
}

void CortBox(RECT *rect, uint32_t col)
{
	//Get rect
	SDL_Rect destRect = RectToSDLRectScaled(rect);
	
	//Set colour and draw
	const unsigned char col_red = col & 0x0000FF;
	const unsigned char col_green = (col & 0x00FF00) >> 8;
	const unsigned char col_blue = (col & 0xFF0000) >> 16;
	SDL_SetRenderDrawColor(gRenderer, col_red, col_green, col_blue, 0xFF);
	SDL_RenderFillRect(gRenderer, &destRect);
}

void CortBox2(RECT *rect, uint32_t col, Surface_Ids surf_no)
{
	//Get rect
	SDL_Rect destRect = RectToSDLRectScaled(rect);

	//Set colour and draw
	const unsigned char col_red = col & 0x0000FF;
	const unsigned char col_green = (col & 0x00FF00) >> 8;
	const unsigned char col_blue = (col & 0xFF0000) >> 16;
	const unsigned char col_alpha = (col & 0xFF0000) >> 24;
	SDL_FillRect(surf[surf_no].surface, &destRect, SDL_MapRGBA(surf[surf_no].surface->format, col_red, col_green, col_blue, col_alpha));
	surf[surf_no].needs_updating = true;
}

int SubpixelToScreenCoord(int coord)
{
	return coord * magnification / 0x200;
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

	if (hfont == NULL)
		hfont = CreateFontA(fontHeight, fontWidth, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, charset, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, NULL);

	if (hfont != NULL)
	{
		HDC hdc = CreateCompatibleDC(NULL);

		if (hdc != NULL)
		{
			SelectObject(hdc, hfont);
			const DWORD size = GetFontData(hdc, 0, 0, NULL, 0);

			if (size != GDI_ERROR)
			{
				buffer = new unsigned char[size];

				if (data_size != NULL)
					*data_size = size;

				if (GetFontData(hdc, 0, 0, buffer, size) != size)
				{
					delete[] buffer;
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
	//Get font size
	unsigned int fontWidth, fontHeight;

	// The original did this, but Windows would downscale it to 5/10 anyway.
/*	if (magnification == 1)
	{
		fontWidth = 6;
		fontHeight = 12;
	}
	else
	{*/
		fontWidth = 5 * magnification;
		fontHeight = 10 * magnification;
//	}
	
#ifdef WINDOWS
	// Actually use the font Config.dat specifies
	size_t data_size;
	unsigned char *data = GetFontFromWindows(&data_size, font_name, fontWidth, fontHeight);

	if (data != NULL)
	{
		gFont = LoadFontFromData(data, data_size, fontWidth, fontHeight);

		delete[] data;

		if (gFont)
			return;
	}
#endif
	// Fall back on the built-in fonts
	(void)font_name;

	//Open Font.ttf
	char path[PATH_LENGTH];
#ifdef JAPANESE
	sprintf(path, "%s/font/msgothic.ttc", gModulePath);
#else
	sprintf(path, "%s/font/cour.ttf", gModulePath);
#endif

	gFont = LoadFont(path, fontWidth, fontHeight);
}

void PutText(int x, int y, const char *text, uint32_t color)
{
	int surface_width, surface_height;
	SDL_GetRendererOutputSize(gRenderer, &surface_width, &surface_height);

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, surface_width, surface_height, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

	DrawText(gFont, surface, x * magnification, y * magnification, color, text, strlen(text));

	SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(gRenderer, screen_texture, NULL, NULL);
	SDL_DestroyTexture(screen_texture);
}

void PutText2(int x, int y, const char *text, uint32_t color, Surface_Ids surf_no)
{
	DrawText(gFont, surf[surf_no].surface, x * magnification, y * magnification, color, text, strlen(text));
	surf[surf_no].needs_updating = true;
}

void EndTextObject()
{
	//Destroy font
	UnloadFont(gFont);
	gFont = NULL;
}
