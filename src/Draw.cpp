#include <stddef.h>
#include "Types.h"
#include "CommonDefines.h"
#include <stdio.h>
#include <stdint.h>

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

#include "WindowsWrapper.h"

#include "Draw.h"
#include "Font.h"
#include "Tags.h"
#include "Resource.h"

RECT grcGame = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
RECT grcFull = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

int magnification;
bool fullscreen;

SURFACE surf[SURFACE_ID_MAX];

FontObject *gFont;

#define FRAMERATE 20

bool Flip_SystemTask()
{
	while (true)
	{
		if (!SystemTask())
			return false;

		//Framerate limiter
		static uint32_t timePrev;
		const uint32_t timeNow = SDL_GetTicks();

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

	SDL_RenderPresent(gRenderer);
	return true;
}

bool StartDirectDraw(int lMagnification, int lColourDepth)
{
	//Initialize rendering
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	
	//Create renderer
	if (gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED))
	{
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
				magnification = 2;
				fullscreen = true;
				SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
				break;
		}
		
	}
	
	return true;
}

void EndDirectDraw()
{
	//Quit sub-system
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	
	//Release all surfaces
	for (int i = 0; i < SURFACE_ID_MAX; i++)
		ReleaseSurface(i);
}

static bool IsEnableBitmap(SDL_RWops *fp)
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
	//Release the surface we want to release
	if (surf[s].in_use)
	{
		SDL_DestroyTexture(surf[s].texture);
		SDL_FreeSurface(surf[s].surface);
		surf[s].in_use = false;
	}
}

bool MakeSurface_Generic(int bxsize, int bysize, int surf_no)
{
	bool success = false;

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
					surf[surf_no].in_use = true;
					success = true;
				}
			}
		}
	}

	return success;
}

static void FlushSurface(int surf_no)
{
	unsigned char *raw_pixels;
	int pitch;
	SDL_LockTexture(surf[surf_no].texture, NULL, (void**)&raw_pixels, &pitch);

	for (int h = 0; h < surf[surf_no].surface->h; ++h)
	{
		for (int w = 0; w < surf[surf_no].surface->w; ++w)
		{
			unsigned char *src_pixel = (unsigned char*)surf[surf_no].surface->pixels + (h * surf[surf_no].surface->pitch) + (w * 4);
			unsigned char *dst_pixel = (unsigned char*)raw_pixels + (h * pitch) + (w * 4);

			dst_pixel[0] = src_pixel[0];
			dst_pixel[1] = src_pixel[1];
			dst_pixel[2] = src_pixel[2];

			if (src_pixel[0] || src_pixel[1] || src_pixel[2])	// Colour-key
				dst_pixel[3] = 0xFF;
			else
				dst_pixel[3] = 0;
		}
	}

	SDL_UnlockTexture(surf[surf_no].texture);
}

static bool LoadBitmap(SDL_RWops *fp, int surf_no, bool create_surface)
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
			SDL_Surface *surface = SDL_LoadBMP_RW(fp, 1);

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
						SDL_FreeSurface(surface);
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

								const unsigned long *src_ptr = (unsigned long*)src_row;
								unsigned long *dst_ptr = (unsigned long*)dst_row;

								for (int w = 0; w < converted_surface->w; ++w)
								{
									const unsigned long src_pixel = *src_ptr++;

									for (int i = 0; i < magnification; ++i)
										*dst_ptr++ = src_pixel;
								}

								for (int i = 1; i < magnification; ++i)
									memcpy(dst_row + i * surf[surf_no].surface->pitch, dst_row, surf[surf_no].surface->w * sizeof(unsigned long));
							}

							SDL_FreeSurface(converted_surface);
							surf[surf_no].needs_updating = true;
							printf(" ^ Successfully loaded\n");
							success = true;
						}
					}
				}

				SDL_FreeSurface(surface);
			}
		}
	}

	return success;
}

static bool LoadBitmap_File(const char *name, int surf_no, bool create_surface)
{
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
		}
		else
		{
			printf("Loading surface (as .pbm) from %s for surface id %d\n", path, surf_no);
			if (LoadBitmap(fp, surf_no, create_surface))
				return true;
		}
	}
	
	//Attempt to load BMP
	sprintf(path, "%s/%s.bmp", gDataPath, name);
	fp = SDL_RWFromFile(path, "rb");
	if (fp)
	{
		printf("Loading surface (as .bmp) from %s for surface id %d\n", path, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return true;
	}
	
	printf("Failed to open file %s\n", name);
	return false;
}

static bool LoadBitmap_Resource(const char *res, int surf_no, bool create_surface)
{
	SDL_RWops *fp = FindResource(res);
	
	if (fp)
	{
		printf("Loading surface from resource %s for surface id %d\n", res, surf_no);
		if (LoadBitmap(fp, surf_no, create_surface))
			return true;
	}
	
	printf("Failed to open resource %s\n", res);
	return false;
}

bool MakeSurface_File(const char *name, int surf_no)
{
	return LoadBitmap_File(name, surf_no, true);
}

bool MakeSurface_Resource(const char *res, int surf_no)
{
	return LoadBitmap_Resource(res, surf_no, true);
}

bool ReloadBitmap_File(const char *name, int surf_no)
{
	return LoadBitmap_File(name, surf_no, false);
}

bool ReloadBitmap_Resource(const char *res, int surf_no)
{
	return LoadBitmap_Resource(res, surf_no, false);
}

SDL_Rect RectToSDLRect(RECT *rect)
{
	SDL_Rect SDLRect = {rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top};
	if (SDLRect.w < 0)
		SDLRect.w = 0;
	if (SDLRect.h < 0)
		SDLRect.h = 0;
	return SDLRect;
}

void BackupSurface(int surf_no, RECT *rect)
{
	//Get renderer size
	int w, h;
	SDL_GetRendererOutputSize(gRenderer, &w, &h);

	//Get texture of what's currently rendered on screen
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);

	//Get rects
	SDL_Rect frameRect = RectToSDLRect(rect);
	frameRect = {frameRect.x * magnification, frameRect.y * magnification, frameRect.w * magnification, frameRect.h * magnification};

	SDL_BlitSurface(surface, &frameRect, surf[surf_no].surface, &frameRect);
	surf[surf_no].needs_updating = true;

	//Free surface
	SDL_FreeSurface(surface);
}

static void DrawBitmap(RECT *rcView, int x, int y, RECT *rect, int surf_no, bool transparent)
{
	if (surf[surf_no].needs_updating)
	{
		FlushSurface(surf_no);
		surf[surf_no].needs_updating = false;
	}

	//Get SDL_Rects
	SDL_Rect clipRect = RectToSDLRect(rcView);
	
	SDL_Rect frameRect = RectToSDLRect(rect);
	frameRect = {frameRect.x * magnification, frameRect.y * magnification, frameRect.w * magnification, frameRect.h * magnification};
	
	//Get dest rect
	SDL_Rect destRect = {x * magnification, y * magnification, frameRect.w, frameRect.h};
	
	//Set cliprect
	clipRect = {clipRect.x * magnification, clipRect.y * magnification, clipRect.w * magnification, clipRect.h * magnification};
	SDL_RenderSetClipRect(gRenderer, &clipRect);
	
	SDL_SetTextureBlendMode(surf[surf_no].texture, transparent ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

	//Draw to screen
	if (SDL_RenderCopy(gRenderer, surf[surf_no].texture, &frameRect, &destRect) < 0)
		printf("Failed to draw texture %d\nSDL Error: %s\n", surf_no, SDL_GetError());
	
	//Undo cliprect
	SDL_RenderSetClipRect(gRenderer, NULL);
}

void PutBitmap3(RECT *rcView, int x, int y, RECT *rect, int surf_no) //Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, true);
}

void PutBitmap4(RECT *rcView, int x, int y, RECT *rect, int surf_no) //No Transparency
{
	DrawBitmap(rcView, x, y, rect, surf_no, false);
}

void Surface2Surface(int x, int y, RECT *rect, int to, int from)
{
	//Get rects
	SDL_Rect rcSet = {x * magnification, y * magnification, (rect->right - rect->left) * magnification, (rect->bottom - rect->top) * magnification};
	SDL_Rect frameRect = RectToSDLRect(rect);
	frameRect = {frameRect.x * magnification, frameRect.y * magnification, frameRect.w * magnification, frameRect.h * magnification};

	SDL_BlitSurface(surf[from].surface, &frameRect, surf[to].surface, &rcSet);
	surf[to].needs_updating = true;
}

void CortBox(RECT *rect, uint32_t col)
{
	//Get rect
	SDL_Rect destRect = RectToSDLRect(rect);
	destRect = {destRect.x * magnification, destRect.y * magnification, destRect.w * magnification, destRect.h * magnification};
	
	//Set colour and draw
	SDL_SetRenderDrawColor(gRenderer, (col & 0xFF0000) >> 16, (col & 0x00FF00) >> 8, col & 0x0000FF, 0xFF);
	SDL_RenderFillRect(gRenderer, &destRect);
}

void CortBox2(RECT *rect, uint32_t col, int surf_no)
{
	//Get rect
	SDL_Rect destRect = RectToSDLRect(rect);
	destRect = {destRect.x * magnification, destRect.y * magnification, destRect.w * magnification, destRect.h * magnification};

	const unsigned char col_red = (col & 0xFF0000) >> 16;
	const unsigned char col_green = (col & 0x00FF00) >> 8;
	const unsigned char col_blue = col & 0x0000FF;
	SDL_FillRect(surf[surf_no].surface, &destRect, SDL_MapRGB(surf[surf_no].surface->format, col_red, col_green, col_blue));
	surf[surf_no].needs_updating = true;
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

void PutText2(int x, int y, const char *text, uint32_t color, int surf_no)
{
	DrawText(gFont, surf[surf_no].surface, x * magnification, y * magnification, color, text, strlen(text));
	surf[surf_no].needs_updating = true;
}

void EndTextObject()
{
	//Destroy font
	UnloadFont(gFont);
	gFont = nullptr;
}
