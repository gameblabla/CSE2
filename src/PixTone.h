#pragma once
#include <stdint.h>

void MakeWaveTables();
bool LoadPxt(char *name, uint8_t **buf, size_t *length);
