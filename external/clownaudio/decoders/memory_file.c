#include "memory_file.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct MemoryFile
{
	unsigned char *data;
	size_t size;
	size_t current_offset;
	bool free_buffer;
};

MemoryFile* MemoryFile_fopen_from(unsigned char *data, size_t size, bool free_buffer)
{
	MemoryFile *memory_file = malloc(sizeof(MemoryFile));

	memory_file->data = data;
	memory_file->size = size;
	memory_file->current_offset = 0;
	memory_file->free_buffer = free_buffer;

	return memory_file;
}

unsigned char* MemoryFile_fopen_to(const char* const file_path, size_t *out_size)
{
	unsigned char *data = NULL;

	FILE *file = fopen(file_path, "rb");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		const size_t size = ftell(file);
		rewind(file);

		data = malloc(size);

		fread(data, 1, size, file);

		fclose(file);

		if (out_size)
			*out_size = size;
	}

	return data;
}

MemoryFile* MemoryFile_fopen(const char* const file_path)
{
	MemoryFile *memory_file = NULL;

	size_t size;
	unsigned char *data = MemoryFile_fopen_to(file_path, &size);

	if (data)
		memory_file = MemoryFile_fopen_from(data, size, true);

	return memory_file;
}

int MemoryFile_fclose(MemoryFile *file)
{
	if (file->free_buffer)
		free(file->data);

	free(file);

	return 0;
}

size_t MemoryFile_fread(void *output, size_t size, size_t count, MemoryFile *file)
{
	const size_t elements_remaining = (file->size - file->current_offset) / size;

	if (count > elements_remaining)
		count = elements_remaining;

	memcpy(output, file->data + file->current_offset, size * count);

	file->current_offset += size * count;

	return count;
}

int MemoryFile_fseek(MemoryFile *file, long offset, int origin)
{
	switch (origin)
	{
		case SEEK_SET:
		{
			file->current_offset = offset;
			break;
		}
		case SEEK_CUR:
		{
			file->current_offset += offset;
			break;
		}
		case SEEK_END:
		{
			file->current_offset = file->size + offset;
			break;
		}
		default:
		{
			return -1;
		}
	}

	return 0;
}

long MemoryFile_ftell(MemoryFile *file)
{
	return (long)file->current_offset;	// BAD, C standard! BAD!
}
