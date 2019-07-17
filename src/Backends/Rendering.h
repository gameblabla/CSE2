#pragma once

#include "SDL.h"

#include "../WindowsWrapper.h"

#include "../Font.h"

struct Backend_Surface;

BOOL Backend_Init(SDL_Renderer *renderer);
void Backend_Deinit(void);
void Backend_DrawScreen(void);
Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height);
void Backend_FreeSurface(Backend_Surface *surface);
void Backend_LoadPixels(Backend_Surface *surface, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int pitch);
void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend);
void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL alpha_blend);
void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue);
void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect);
void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour);
void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour);
void Backend_HandleDeviceLoss(void);
void Backend_HandleWindowResize(void);
