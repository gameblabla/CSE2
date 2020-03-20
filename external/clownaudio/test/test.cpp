#include <stddef.h>
#include <stdint.h>

#include <clownaudio/mixer.h>
#include <clownaudio/playback.h>

#include "glad/glad.h"
#include "SDL.h"
#include "tinydir.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

typedef struct SoundDataListEntry
{
	ClownAudio_SoundData *sound_data;

	struct SoundDataListEntry *next;
} SoundDataListEntry;

typedef struct SoundListEntry
{
	ClownAudio_Sound sound;

	struct SoundListEntry *next;
} SoundListEntry;

static SoundDataListEntry *sound_data_list_head;
static SoundListEntry *sound_list_head;

static void StreamCallback(void *user_data, float *output_buffer, size_t frames_to_do)
{
	ClownAudio_Mixer *mixer = (ClownAudio_Mixer*)user_data;

	for (size_t i = 0; i < frames_to_do * CLOWNAUDIO_STREAM_CHANNEL_COUNT; ++i)
		output_buffer[i] = 0.0f;

	ClownAudio_MixSamples(mixer, output_buffer, frames_to_do);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("clownaudio test program\n\nUsage: %s [intro file path] [loop file path (optional)]\n\n", argv[0]);
		return 0;
	}

	const char *file_directory = argc > 1 ? argv[1] : ".";

	///////////////////////////
	// Initialise clownaudio //
	///////////////////////////

	ClownAudio_InitPlayback();

	ClownAudio_Mixer *mixer = ClownAudio_CreateMixer(CLOWNAUDIO_STREAM_SAMPLE_RATE);

	ClownAudio_Stream *stream = ClownAudio_CreateStream(StreamCallback, mixer);
	ClownAudio_ResumeStream(stream);

	/////////////////////
	// Initialise SDL2 //
	/////////////////////

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);

    const char* glsl_version = "#version 150 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window *window = SDL_CreateWindow("clownaudio test program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window != NULL)
	{
		SDL_GLContext gl_context = SDL_GL_CreateContext(window);

		SDL_GL_MakeCurrent(window, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync

		if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			// Check if the platform supports OpenGL 3.2
			if (GLAD_GL_VERSION_3_2)
			{
				///////////////////////////
				// Initialise Dear ImGui //
				///////////////////////////

				IMGUI_CHECKVERSION();
				ImGui::CreateContext();

				ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
				ImGui_ImplOpenGL3_Init(glsl_version);

				// Load sound
				ClownAudio_SoundDataConfig data_config;
				ClownAudio_InitSoundDataConfig(&data_config);

				ClownAudio_SoundConfig sound_config;
				ClownAudio_InitSoundConfig(&sound_config);

				ClownAudio_SoundData *selected_sound_data = NULL;
				ClownAudio_Sound selected_sound = 0;

				int intro_file = 0;
				int loop_file = 0;

				// tinydir stuff
				tinydir_dir dir;
				tinydir_open_sorted(&dir, file_directory);

				tinydir_file *files = (tinydir_file*)malloc(sizeof(tinydir_file) * dir.n_files);
				size_t files_total = 0;

				for (size_t i = 0; i < dir.n_files; ++i)
				{
					tinydir_readfile_n(&dir, &files[files_total], i);

					if (!files[files_total].is_dir)
						++files_total;
				}

				tinydir_close(&dir);

				bool quit = false;

				while (!quit)
				{
					SDL_Event event;
					while (SDL_PollEvent(&event))
					{
						ImGui_ImplSDL2_ProcessEvent(&event);

						if (event.type == SDL_QUIT)
							quit = true;
					}

					ImGui_ImplOpenGL3_NewFrame();
					ImGui_ImplSDL2_NewFrame(window);
					ImGui::NewFrame();

					ImGui::Begin("Sound Data Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize);
						struct FuncHolder
						{
							static bool ItemGetter(void* data, int idx, const char** out_str)
							{
								if (idx == 0)
								{
									*out_str = "No file";
								}
								else
								{
									tinydir_file *files = (tinydir_file*)data;
									*out_str = files[idx - 1].name;
								}

								return true;
							}
						};

						ImGui::Combo("Intro file", &intro_file, &FuncHolder::ItemGetter, files, files_total + 1);
						ImGui::Combo("Loop file", &loop_file, &FuncHolder::ItemGetter, files, files_total + 1);

						ImGui::Checkbox("Predecode", &data_config.predecode);
						ImGui::Checkbox("Must predecode", &data_config.must_predecode);
						ImGui::Checkbox("Dynamic sample rate", &data_config.dynamic_sample_rate);

						if (ImGui::Button("Load sound data"))
						{
							SoundDataListEntry *entry = (SoundDataListEntry*)malloc(sizeof(SoundDataListEntry));

							entry->sound_data = ClownAudio_LoadSoundDataFromFiles(intro_file == 0 ? NULL : files[intro_file - 1].path, loop_file == 0 ? NULL : files[loop_file - 1].path, &data_config);
							entry->next = sound_data_list_head;

							sound_data_list_head = entry;
						}
					ImGui::End();

					ImGui::Begin("Sound Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize);
						ImGui::Checkbox("Loop", &sound_config.loop);
						ImGui::Checkbox("Do not free when done", &sound_config.do_not_free_when_done);
						ImGui::Checkbox("Dynamic sample rate", &sound_config.dynamic_sample_rate);

						if (ImGui::Button("Create sound"))
						{
							ClownAudio_Sound sound = ClownAudio_CreateSound(mixer, selected_sound_data, &sound_config);

							if (sound != 0)
							{
								SoundListEntry *entry = (SoundListEntry*)malloc(sizeof(SoundListEntry));

								entry->sound = sound;
								entry->next = sound_list_head;

								sound_list_head = entry;
							}
						}
					ImGui::End();

					ImGui::Begin("Sound Data", NULL);
						for (SoundDataListEntry *entry = sound_data_list_head; entry != NULL; entry = entry->next)
						{
							char name[32];
							sprintf(name, "Sound data %p", entry->sound_data);
							if (ImGui::Selectable(name, selected_sound_data == entry->sound_data))
								selected_sound_data = entry->sound_data;
						}

						if (ImGui::Button("Destroy"))
						{
							ClownAudio_UnloadSoundData(selected_sound_data);

							for (SoundDataListEntry **entry = &sound_data_list_head; *entry != NULL; entry = &(*entry)->next)
							{
								if ((*entry)->sound_data == selected_sound_data)
								{
									SoundDataListEntry *next_sound_data = (*entry)->next;
									free(*entry);
									*entry = next_sound_data;
									selected_sound_data = *entry == NULL ? NULL : (*entry)->sound_data;
									break;
								}
							}
						}
					ImGui::End();

					ImGui::Begin("Sounds", NULL);
						for (SoundListEntry *entry = sound_list_head; entry != NULL; entry = entry->next)
						{
							int status = ClownAudio_GetSoundStatus(mixer, entry->sound);

							char name[32];

							switch (status)
							{
								case 0:
									sprintf(name, "Sound %u - Playing", entry->sound);
									break;

								case 1:
									sprintf(name, "Sound %u - Paused", entry->sound);
									break;

								case -1:
									sprintf(name, "Sound %u - Freed", entry->sound);
									break;

							}

							if (ImGui::Selectable(name, selected_sound == entry->sound))
								selected_sound = entry->sound;
						}

						if (ImGui::Button("Destroy"))
						{
							ClownAudio_DestroySound(mixer, selected_sound);

							for (SoundListEntry **entry = &sound_list_head; *entry != NULL; entry = &(*entry)->next)
							{
								if ((*entry)->sound == selected_sound)
								{
									SoundListEntry *next_sound = (*entry)->next;
									free(*entry);
									*entry = next_sound;
									break;
								}
							}
						}
					ImGui::End();

					ImGui::Begin("Sound Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);
						if (ImGui::Button("Pause"))
							ClownAudio_PauseSound(mixer, selected_sound);

						if (ImGui::Button("Unpause"))
							ClownAudio_UnpauseSound(mixer, selected_sound);

						if (ImGui::Button("Rewind"))
							ClownAudio_RewindSound(mixer, selected_sound);

						if (ImGui::Button("Fade-out"))
							ClownAudio_FadeOutSound(mixer, selected_sound, 5 * 1000);

						if (ImGui::Button("Fade-in"))
							ClownAudio_FadeInSound(mixer, selected_sound, 5 * 1000);

						if (ImGui::Button("Cancel fade"))
							ClownAudio_CancelFade(mixer, selected_sound);

						static float volume=1.0f;
						ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.3f");
						ClownAudio_SetSoundVolume(mixer, selected_sound, volume, volume);

						static int sample_rate = 48000;
						ImGui::InputInt("Sample rate", &sample_rate);

						if (ImGui::Button("Apply sample rate"))
							ClownAudio_SetSoundSampleRate(mixer, selected_sound, sample_rate, sample_rate);
					ImGui::End();

					ImGui::ShowDemoWindow();

					ImGui::Render();
					glClear(GL_COLOR_BUFFER_BIT);
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					SDL_GL_SwapWindow(window);
				}
			}
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		SDL_GL_DeleteContext(gl_context);
		SDL_DestroyWindow(window);
	}

	SDL_Quit();

	ClownAudio_DestroyStream(stream);

	ClownAudio_DestroyMixer(mixer);

	ClownAudio_DeinitPlayback();

	return 0;
}
