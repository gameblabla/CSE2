#pragma once

#include <stddef.h>

#include "Backends/Rendering.h"

typedef struct FontObject FontObject;

FontObject* LoadFontFromData(const unsigned char *data, size_t data_size, size_t cell_width, size_t cell_height);
FontObject* LoadFont(const char *font_filename, size_t cell_width, size_t cell_height);
void DrawText(FontObject *font_object, RenderBackend_Surface *surface, int x, int y, unsigned long colour, const char *string);
void UnloadFont(FontObject *font_object);
