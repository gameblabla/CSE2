/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

//#define STB_LEAKCHECK_IMPLEMENTATION
//#include "stb_leakcheck.h"

#include "clownaudio/clownaudio.h"

static void FileToMemory(const char *filename, unsigned char **buffer, size_t *size)
{
	*buffer = NULL;
	*size = 0;

	FILE *file = fopen(filename, "rb");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size_t _size = ftell(file);
		rewind(file);

		unsigned char *_buffer = (unsigned char*)malloc(_size);

		if (_buffer != NULL)
		{
			fread(_buffer, 1, _size, file);
			*buffer = _buffer;
			*size = _size;
		}

		fclose(file);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		printf("clownaudio test program\n\nUsage: %s [intro file] [loop file (optional)]\n\n", argv[0]);
		return 0;
	}

	if (ClownAudio_Init())
	{
		printf("Initialised mixer\n");
		fflush(stdout);

		unsigned char *file_buffers[2];
		size_t file_sizes[2];
		if (argc == 3)
		{
			FileToMemory(argv[1], &file_buffers[0], &file_sizes[0]);
			FileToMemory(argv[2], &file_buffers[1], &file_sizes[1]);
		}
		else if (argc == 2)
		{
			FileToMemory(argv[1], &file_buffers[0], &file_sizes[0]);
			file_buffers[1] = NULL;
			file_sizes[1] = 0;
		}

		ClownAudio_SoundDataConfig config;
		ClownAudio_InitSoundDataConfig(&config);
		ClownAudio_SoundData *sound_data = ClownAudio_LoadSoundData(file_buffers[0], file_sizes[0], file_buffers[1], file_sizes[1], &config);

		if (sound_data != NULL)
		{
			printf("Loaded sound data\n");
			fflush(stdout);

			ClownAudio_SoundConfig config2;
			ClownAudio_InitSoundConfig(&config2);
			config2.loop = true;
			ClownAudio_Sound instance = ClownAudio_CreateSound(sound_data, &config2);
			ClownAudio_UnpauseSound(instance);

			if (instance != 0)
			{
				printf("Started sound\n");
				fflush(stdout);

				printf("\n"
				       "Controls:\n"
				       " q                - Quit\n"
				       " r                - Rewind sound\n"
				       " o [duration]     - Fade-out sound (milliseconds)\n"
				       " i [duration]     - Fade-in sound (milliseconds)\n"
				       " c                - Cancel fade\n"
				       " u [rate]         - Set sample-rate (Hz)\n"
				       " p                - Pause/unpause sound\n"
				       " v [left] [right] - Set sound volume (0.0-1.0)\n"
				);
				fflush(stdout);

				bool pause = false;

				bool exit = false;
				while (!exit)
				{
					char buffer[128];
					fgets(buffer, sizeof(buffer), stdin);

					char mode;
					while (sscanf(buffer, "%c", &mode) != 1);

					switch (mode)
					{
						case 'q':
							printf("Quitting\n");
							fflush(stdout);

							exit = true;
							break;

						case 'r':
							printf("Rewinding sound\n");
							fflush(stdout);

							ClownAudio_RewindSound(instance);
							break;

						case 'o':
						{
							unsigned int param;
							if (sscanf(buffer, "%c %u", &mode, &param) != 2)
								param = 1000 * 2;

							printf("Fading-out sound over %u milliseconds\n", param);
							fflush(stdout);

							ClownAudio_FadeOutSound(instance, param);
							break;
						}

						case 'i':
						{
							unsigned int param;
							if (sscanf(buffer, "%c %u", &mode, &param) != 2)
								param = 1000 * 2;

							printf("Fading-in sound over %u milliseconds\n", param);
							fflush(stdout);

							ClownAudio_FadeInSound(instance, param);
							break;
						}

						case 'c':
							printf("Cancelling fade\n");
							fflush(stdout);

							ClownAudio_CancelFade(instance);
							break;

						case 'u':
						{
							unsigned int param;
							if (sscanf(buffer, "%c %u", &mode, &param) != 2)
								param = 8000;

							printf("Setting sample-rate to %uHz\n", param);
							fflush(stdout);

							ClownAudio_SetSoundSampleRate(instance, param, param);
							break;
						}

						case 'p':
							if (pause)
							{
								printf("Unpausing sound\n");
								fflush(stdout);

								ClownAudio_UnpauseSound(instance);
							}
							else
							{
								printf("Pausing sound\n");
								fflush(stdout);

								ClownAudio_PauseSound(instance);
							}

							pause = !pause;

							break;

						case 'v':
						{
							float volume_left, volume_right;
							int values_read = sscanf(buffer, "%c %f %f", &mode, &volume_left, &volume_right);

							if (values_read == 1)
								volume_left = volume_right = 1.0f;
							else if (values_read == 2)
								volume_right = volume_left;

							printf("Setting volume to %f left, %f right\n", volume_left, volume_right);
							fflush(stdout);

							ClownAudio_SetSoundVolume(instance, volume_left, volume_right);
							break;
						}
					}
				}

				printf("Stopping sound\n");
				fflush(stdout);
				ClownAudio_DestroySound(instance);
			}
			else
			{
				printf("Couldn't start sound\n");
			}

			printf("Unloading sound data\n");
			fflush(stdout);
			ClownAudio_UnloadSoundData(sound_data);

			free(file_buffers[1]);
			free(file_buffers[0]);
		}
		else
		{
			printf("Couldn't load sound data\n");
		}

		printf("Deinitialising mixer\n");
		fflush(stdout);
		ClownAudio_Deinit();
	}
	else
	{
		printf("Couldn't initialise mixer\n");
	}


//	stb_leakcheck_dumpmem();

	return 0;
}
