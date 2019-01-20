//Bin2h by Cucky
//Converts files to the .h's expected by Cave Story Engine for resources.

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>

const char *h1 = "#pragma once\n";
const char *end = "\n};\n";

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::string filename = argv[1];

		//Remove directory if present.
		//Do this before extension removal incase directory has a period character.
		const size_t last_slash_idx = filename.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			filename.erase(0, last_slash_idx + 1);
		}

		//Remove extension if present.
		const size_t period_idx = filename.rfind('.');
		if (std::string::npos != period_idx)
		{
			filename.erase(period_idx);
		}

		//Open files
		FILE *from = fopen(argv[1], "rb");
		FILE *to = fopen(argv[2], "wb");

		//Write "#pragma once"
		fwrite(h1, strlen(h1), 1, to);

		//Get from file's size
		fseek(from, 0, SEEK_END);
		size_t fromSize = ftell(from);
		rewind(from);

		//Write definition
		char defin[0x400];
		sprintf(defin, "const unsigned char r%s[%d] = {", filename.c_str(), fromSize);
		fwrite(defin, strlen(defin), 1, to);

		//Write actual data
		char tLine[0x400];

		for (size_t v = 0; v < fromSize; v++)
		{
			if ((v & 0xF) == 0)
				strcpy(tLine, "\n\x09");
			uint8_t val;
			fread(&val, 1, 1, from);
			sprintf(tLine, "%s0x%2.2X, ", tLine, val);

			if ((((v + 1) & 0xF) == 0))
				fwrite(tLine, strlen(tLine), 1, to);
		}

		//Write end
		fwrite(end, strlen(end), 1, to);

		//Close files
		fclose(from);
		fclose(to);
	}

	return 0;
}
