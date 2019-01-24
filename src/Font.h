#pragma once

#include <iconv.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "SDL.h"

typedef struct FontObject
{
	FT_Library library;
	FT_Face face;
#ifndef DISABLE_FONT_ANTIALIASING
	bool lcd_mode;
#endif
#ifdef JAPANESE
	iconv_t conv;
#endif
} FontObject;

FontObject* LoadFont(unsigned int cell_width, unsigned int cell_height, char *font_filename);
void DrawText(FontObject *font_object, SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, unsigned long colour, const char *string, size_t string_length);
void UnloadFont(FontObject *font_object);
