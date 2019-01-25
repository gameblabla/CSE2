#include "Font.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <iconv.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_BITMAP_H

#include "SDL.h"

// Uncomment for that authentic pre-Windows Vista feel
//#define DISABLE_FONT_ANTIALIASING

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

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

static unsigned long UTF8ToCode(const unsigned char *string, unsigned int *bytes_read)
{
	unsigned int length;
	unsigned long charcode;

	if ((string[0] & 0x80) == 0)
	{
		length = 1;
		charcode = string[0] & 0x7F;
	}
	else if ((string[0] & 0xE0) == 0xC0)
	{
		length = 2;
		charcode = ((string[0] & ~0xE0) << 6) | (string[1] & 0x3F);
	}
	else if ((string[0] & 0xF0) == 0xE0)
	{
		length = 3;
		charcode = ((string[0] & ~0xF0) << (6 * 2)) | ((string[1] & 0x3F) << 6) | (string[2] & 0x3F);
	}
	else //if (string[0] & 0xF8 == 0xF0)
	{
		length = 4;
		charcode = ((string[0] & ~0xF8) << (6 * 3)) | ((string[1] & 0x3F) << (6 * 2)) | ((string[2] & 0x3F) << 6) | (string[3] & 0x3F);
	}

	if (bytes_read)
		*bytes_read = length;

	return charcode;
}

FontObject* LoadFont(unsigned int cell_width, unsigned int cell_height, char *font_filename)
{
	FontObject *font_object = (FontObject*)malloc(sizeof(FontObject));

	FT_Init_FreeType(&font_object->library);

#ifndef DISABLE_FONT_ANTIALIASING
	font_object->lcd_mode = FT_Library_SetLcdFilter(font_object->library, FT_LCD_FILTER_DEFAULT) != FT_Err_Unimplemented_Feature;
#endif

	FT_New_Face(font_object->library, font_filename, 0, &font_object->face);

	unsigned int best_cell_width = 0;
	unsigned int best_cell_height = 0;
	unsigned int best_pixel_width = 0;
	unsigned int best_pixel_height = 0;

	for (unsigned int i = 0;; ++i)
	{
		FT_Set_Pixel_Sizes(font_object->face, i, i);

		const unsigned int current_cell_width = font_object->face->size->metrics.max_advance / 64;
		const unsigned int current_cell_height = font_object->face->size->metrics.height / 64;

		if (current_cell_width > cell_width && current_cell_height > cell_height)
		{
			break;
		}
		else
		{
			if (current_cell_width <= cell_width)
			{
				best_pixel_width = i;
				best_cell_width = current_cell_width;
			}
			if (current_cell_height <= cell_height)
			{
				best_pixel_height = i;
				best_cell_height = current_cell_height;
			}
		}
	}

#ifdef JAPANESE
	best_pixel_width = 0;	// Cheap hack to make the font square
#endif

	FT_Set_Pixel_Sizes(font_object->face, best_pixel_width, best_pixel_height);

#ifdef JAPANESE
	font_object->conv = iconv_open("UTF-8", "SHIFT-JIS");
#endif

	return font_object;
}

void DrawText(FontObject *font_object, SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, unsigned long colour, const char *string, size_t string_length)
{
	const unsigned char colours[3] = {(unsigned char)(colour >> 16), (unsigned char)(colour >> 8), (unsigned char)colour};

	SDL_Texture *old_render_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);

	int surface_width, surface_height;
	SDL_GetRendererOutputSize(renderer, &surface_width, &surface_height);

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, surface_width, surface_height, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch);
	unsigned char (*surface_buffer)[surface->pitch / 4][4] = (unsigned char (*)[surface->pitch / 4][4])surface->pixels;

	FT_Face face = font_object->face;

	unsigned int pen_x = 0;

	const unsigned char *string_pointer = (unsigned char*)string;
	const unsigned char *string_end = (unsigned char*)string + string_length;

	while (string_pointer != string_end)
	{
#ifdef JAPANESE
		size_t out_size = 4;
		unsigned char out_buffer[4];	// Max UTF-8 length is four bytes
		unsigned char *out_pointer = out_buffer;

		size_t in_size = ((*string_pointer >= 0x81 && *string_pointer <= 0x9F) || (*string_pointer >= 0xE0 && *string_pointer <= 0xEF)) ? 2 : 1;
		unsigned char in_buffer[2];
		unsigned char *in_pointer = in_buffer;

		for (size_t i = 0; i < in_size; ++i)
			in_buffer[i] = string_pointer[i];

		string_pointer += in_size;
	
		iconv(font_object->conv, (char**)&in_pointer, &in_size, (char**)&out_pointer, &out_size);

		const unsigned long val = UTF8ToCode(out_buffer, NULL);
#else
		unsigned int bytes_read;
		const unsigned long val = UTF8ToCode(string_pointer, &bytes_read);
		string_pointer += bytes_read;
#endif

		unsigned int glyph_index = FT_Get_Char_Index(face, val);

#ifndef DISABLE_FONT_ANTIALIASING
		FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | (font_object->lcd_mode ? FT_LOAD_TARGET_LCD : 0));
#else
		FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
#endif

		FT_Bitmap converted;
		FT_Bitmap_New(&converted);
		FT_Bitmap_Convert(font_object->library, &face->glyph->bitmap, &converted, 1);

		const int letter_x = x + pen_x + face->glyph->bitmap_left;
		const int letter_y = y + ((FT_MulFix(face->ascender, face->size->metrics.y_scale) + (64 - 1)) / 64) - (face->glyph->metrics.horiBearingY / 64);

		for (int iy = MAX(-letter_y, 0); letter_y + iy < MIN(letter_y + converted.rows, surface_height); ++iy)
		{
			if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_LCD)
			{
				for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width / 3, surface_width); ++ix)
				{
					const unsigned char (*font_buffer)[converted.pitch / 3][3] = (unsigned char (*)[converted.pitch / 3][3])converted.buffer;

					const unsigned char *font_pixel = font_buffer[iy][ix];
					unsigned char *surface_pixel = surface_buffer[letter_y + iy][letter_x + ix];

					if (font_pixel[0] || font_pixel[1] || font_pixel[2])
					{
						for (unsigned int j = 0; j < 3; ++j)
						{
							const double alpha = pow((font_pixel[j] / 255.0), 1.0 / 1.8);			// Gamma correction
							surface_pixel[j] = (colours[j] * alpha) + (surface_pixel[j] * (1.0 - alpha));	// Alpha blending
						}

						surface_pixel[3] = 0xFF;
					}
				}
			}
			else if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
			{
				for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width, surface_width); ++ix)
				{
					unsigned char (*font_buffer)[converted.pitch] = (unsigned char (*)[converted.pitch])converted.buffer;

					const double alpha = pow((double)font_buffer[iy][ix] / (converted.num_grays - 1), 1.0 / 1.8);			// Gamma-corrected

					unsigned char *surface_pixel = surface_buffer[letter_y + iy][letter_x + ix];

					if (alpha)
					{
						for (unsigned int j = 0; j < 3; ++j)
							surface_pixel[j] = (colours[j] * alpha) + (surface_pixel[j] * (1.0 - alpha));	// Alpha blending

						surface_pixel[3] = 0xFF;
					}
				}
			}
			else if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width, surface_width); ++ix)
				{
					unsigned char (*font_buffer)[converted.pitch] = (unsigned char (*)[converted.pitch])converted.buffer;

					unsigned char *surface_pixel = surface_buffer[letter_y + iy][letter_x + ix];

					if (font_buffer[iy][ix])
					{
						for (unsigned int j = 0; j < 3; ++j)
							surface_pixel[j] = colours[j];

						surface_pixel[3] = 0xFF;
					}
				}
			}
		}

		FT_Bitmap_Done(font_object->library, &converted);

		pen_x += face->glyph->advance.x / 64;
	}

	SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
	SDL_DestroyTexture(screen_texture);
	SDL_SetRenderTarget(renderer, old_render_target);
}

void UnloadFont(FontObject *font_object)
{
#ifdef JAPANESE
	iconv_close(font_object->conv);
#endif
	FT_Done_Face(font_object->face);
	FT_Done_FreeType(font_object->library);
}
