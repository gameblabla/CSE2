//Bin2h by -C-u-c-k-y- Clownypants
//Converts files to the .h's expected by Cave Story Engine for resources.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int result = 0;

	if (argc > 2)
	{
		char *last_forward_slash = strrchr(argv[1], '/');
		char *last_back_slash = strrchr(argv[1], '\\');

		char *last_path_seperator = last_forward_slash > last_back_slash ? last_forward_slash : last_back_slash;

		char *filename_pointer = (last_path_seperator == NULL) ? argv[1] : last_path_seperator + 1;
		char *dot = strchr(filename_pointer, '.');
		size_t filename_length = (dot == NULL) ? strlen(filename_pointer) : dot - filename_pointer;

		char *filename = malloc(filename_length + 1);
		memcpy(filename, filename_pointer, filename_length);
		filename[filename_length] = '\0';

		FILE *in_file = fopen(argv[1], "rb");
		FILE *out_file = fopen(argv[2], "w");

		if (in_file == NULL)
		{
			printf("Couldn't open '%s'\n", argv[1]);
			result = 1;
		}
		else if (out_file == NULL)
		{
			printf("Couldn't open '%s'\n", argv[2]);
			result = 1;
		}
		else
		{
			fseek(in_file, 0, SEEK_END);
			const long in_file_size = ftell(in_file);
			rewind(in_file);
			unsigned char *in_file_buffer = malloc(in_file_size);
			fread(in_file_buffer, 1, in_file_size, in_file);
			fclose(in_file);
			unsigned char *in_file_pointer = in_file_buffer;

			setvbuf(out_file, NULL, _IOFBF, 0x10000);

			fprintf(out_file, "#pragma once\n\nconst unsigned char r%s[0x%lX] = {\n\t", filename, in_file_size);

			for (long i = 0; i < in_file_size - 1; ++i)
			{
				if (i % 16 == 15)
					fprintf(out_file, "0x%02X,\n\t", *in_file_pointer++);
				else
					fprintf(out_file, "0x%02X, ", *in_file_pointer++);
			}

			fprintf(out_file, "0x%02X\n};\n", *in_file_pointer++);

			fclose(out_file);
			free(in_file_buffer);
		}

		free(filename);
	}
	else
	{
		result = 1;
	}

	return result;
}
