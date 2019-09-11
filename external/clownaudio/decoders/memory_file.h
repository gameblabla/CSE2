#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef struct MemoryFile MemoryFile;

MemoryFile* MemoryFile_fopen_from(unsigned char *data, size_t size, bool free_buffer);
unsigned char* MemoryFile_fopen_to(const char* const file_path, size_t *out_size);
MemoryFile* MemoryFile_fopen(const char* const file_path);
int MemoryFile_fclose(MemoryFile *file);

size_t MemoryFile_fread(void *output, size_t size, size_t count, MemoryFile *file);
int MemoryFile_fseek(MemoryFile *file, long offset, int origin);
long MemoryFile_ftell(MemoryFile *file);
