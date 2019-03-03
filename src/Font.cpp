#include "Font.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef JAPANESE
#include <iconv.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_BITMAP_H

#include "File.h"

// Uncomment for that authentic pre-Windows Vista feel
#define DISABLE_FONT_ANTIALIASING

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct FontObject
{
	FT_Library library;
	FT_Face face;
	unsigned char *data;
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

	unsigned int zero_bit = 0;
	for (unsigned char lead_byte = string[0]; zero_bit < 5 && (lead_byte & 0x80); ++zero_bit, lead_byte <<= 1);

	switch (zero_bit)
	{
		case 0:
			// Single-byte character
			length = 1;
			charcode = string[0];
			break;

		case 2:
		case 3:
		case 4:
			length = zero_bit;
			charcode = string[0] & ((1 << (8 - zero_bit)) - 1);

			for (unsigned int i = 1; i < zero_bit; ++i)
			{
				if ((string[i] & 0xC0) == 0x80)
				{
					charcode <<= 6;
					charcode |= string[i] & ~0xC0;
				}
				else
				{
					// Error: Invalid continuation byte
					length = 1;
					charcode = 0xFFFD;
					break;
				}
			}

			break;

		default:
			// Error: Invalid lead byte
			length = 1;
			charcode = 0xFFFD;
			break;

	}

	if (bytes_read)
		*bytes_read = length;

	return charcode;
}

FontObject* LoadFontFromData(const unsigned char *data, size_t data_size, unsigned int cell_width, unsigned int cell_height)
{
	FontObject *font_object = (FontObject*)malloc(sizeof(FontObject));

	FT_Init_FreeType(&font_object->library);

#ifndef DISABLE_FONT_ANTIALIASING
	font_object->lcd_mode = FT_Library_SetLcdFilter(font_object->library, FT_LCD_FILTER_DEFAULT) != FT_Err_Unimplemented_Feature;
#endif

	font_object->data = (unsigned char*)malloc(data_size);
	memcpy(font_object->data, data, data_size);

	FT_Error error = FT_New_Memory_Face(font_object->library, font_object->data, data_size, 0, &font_object->face);

	if (error)
	{
		free(font_object->data);
		FT_Done_FreeType(font_object->library);
		free(font_object);
		return NULL;
	}

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
				best_pixel_width = i;

			if (current_cell_height <= cell_height)
				best_pixel_height = i;
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

FontObject* LoadFont(const char *font_filename, unsigned int cell_width, unsigned int cell_height)
{
	FontObject *font_object = NULL;

	unsigned char *file_buffer;
	const long file_size = LoadFileToMemory(font_filename, &file_buffer);

	if (file_size != -1)
	{
		font_object = LoadFontFromData(file_buffer, file_size, cell_width, cell_height);
		free(file_buffer);
	}

	return font_object;
}

void DrawText(FontObject *font_object, BUFFER_PIXEL *surface, unsigned int surface_width, int x, int y, unsigned long colour, const char *string, size_t string_length)
{
	if (font_object != NULL)
	{
		const unsigned char colours[3] = {(unsigned char)colour, (unsigned char)(colour >> 8), (unsigned char)(colour >> 16)};

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
			const int letter_y = y + ((FT_MulFix(face->ascender, face->size->metrics.y_scale) - face->glyph->metrics.horiBearingY + (64 / 2)) / 64);

			for (int iy = MAX(-letter_y, 0); letter_y + iy < MIN(letter_y + (int)converted.rows, surface->h); ++iy)
			{
				if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_LCD)
				{
					for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width / 3, surface->w); ++ix)
					{
						const unsigned char *font_pixel = converted.buffer + iy * converted.pitch + ix * 3;
						BUFFER_PIXEL *surface_pixel = &surface[(letter_y + iy) * surface_width + (letter_x + ix)];

						if (font_pixel[0] || font_pixel[1] || font_pixel[2])
						{
							const double alpha0 = pow((font_pixel[0] / 255.0), 1.0 / 1.8);			// Gamma correction
							const double alpha1 = pow((font_pixel[1] / 255.0), 1.0 / 1.8);			// Gamma correction
							const double alpha2 = pow((font_pixel[2] / 255.0), 1.0 / 1.8);			// Gamma correction
							
							surface_pixel->r = (colours[0] * alpha0) + (surface_pixel->r * (1.0 - alpha0));	// Alpha blending
							surface_pixel->g = (colours[1] * alpha1) + (surface_pixel->g * (1.0 - alpha1));	// Alpha blending
							surface_pixel->b = (colours[2] * alpha2) + (surface_pixel->b * (1.0 - alpha2));	// Alpha blending
						}
					}
				}
				else if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width, surface->w); ++ix)
					{
						const unsigned char font_pixel = converted.buffer[iy * converted.pitch + ix];

						const double alpha = pow((double)font_pixel / (converted.num_grays - 1), 1.0 / 1.8);			// Gamma-corrected

						unsigned char *surface_pixel = (unsigned char*)surface->pixels + (letter_y + iy) * surface->pitch + (letter_x + ix) * 4;

						if (alpha)
						{
							for (unsigned int j = 0; j < 3; ++j)
								surface_pixel[j] = (unsigned char)((colours[j] * alpha) + (surface_pixel[j] * (1.0 - alpha)));	// Alpha blending

							surface_pixel[3] = 0xFF;
						}
					}
				}
				else if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
				{
					for (int ix = MAX(-letter_x, 0); letter_x + ix < MIN(letter_x + (int)converted.width, surface->w); ++ix)
					{
						const unsigned char font_pixel = converted.buffer[iy * converted.pitch + ix];

						unsigned char *surface_pixel = (unsigned char*)surface->pixels + (letter_y + iy) * surface->pitch + (letter_x + ix) * 4;

						if (font_pixel)
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
	}
}

void UnloadFont(FontObject *font_object)
{
	if (font_object != NULL)
	{
#ifdef JAPANESE
		iconv_close(font_object->conv);
#endif
		FT_Done_Face(font_object->face);
		free(font_object->data);
		FT_Done_FreeType(font_object->library);
		free(font_object);
	}
}
