#include <stddef.h>
#include <stdio.h>

#include <clownaudio/clownaudio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "tinydir.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

typedef struct SoundDataListEntry
{
	ClownAudio_SoundData *sound_data;

	struct SoundDataListEntry *next;
} SoundDataListEntry;

typedef struct SoundListEntry
{
	ClownAudio_SoundID sound_id;
	float master_volume;
	float volume_left;
	float volume_right;

	struct SoundListEntry *next;
} SoundListEntry;

static SoundDataListEntry *sound_data_list_head;
static SoundListEntry *sound_list_head;

int main(int argc, char *argv[])
{
	const char *file_directory = argc > 1 ? argv[1] : ".";

	/////////////////////
	// Initialise GLFW //
	/////////////////////

	if (glfwInit())
	{
		const char *glsl_version = "#version 150 core";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

		GLFWwindow *window = glfwCreateWindow(800, 600, "clownaudio test program", NULL, NULL);

		if (window != NULL)
		{
			glfwMakeContextCurrent(window);
			glfwSwapInterval(1);

			if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				// Check if the platform supports OpenGL 3.2
				if (GLAD_GL_VERSION_3_2)
				{
					///////////////////////////
					// Initialise Dear ImGui //
					///////////////////////////

					IMGUI_CHECKVERSION();
					ImGui::CreateContext();

					ImGui_ImplGlfw_InitForOpenGL(window, true);
					ImGui_ImplOpenGL3_Init(glsl_version);

					///////////////////////////
					// Initialise clownaudio //
					///////////////////////////

					if (ClownAudio_Init())
					{
						ClownAudio_SoundDataConfig data_config;
						ClownAudio_InitSoundDataConfig(&data_config);

						ClownAudio_SoundConfig sound_config;
						ClownAudio_InitSoundConfig(&sound_config);

						SoundDataListEntry *selected_sound_data = NULL;
						SoundListEntry *selected_sound = NULL;

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

						while (!glfwWindowShouldClose(window))
						{
							glfwPollEvents();

							ImGui_ImplOpenGL3_NewFrame();
							ImGui_ImplGlfw_NewFrame();
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

								ImGui::Spacing();

								ImGui::Checkbox("Predecode", &data_config.predecode);
								ImGui::Checkbox("Must predecode", &data_config.must_predecode);
								ImGui::Checkbox("Dynamic sample rate", &data_config.dynamic_sample_rate);

								ImGui::Spacing();

								if (ImGui::Button("Load sound data"))
								{
									ClownAudio_SoundData *sound_data = ClownAudio_LoadSoundDataFromFiles(intro_file == 0 ? NULL : files[intro_file - 1].path, loop_file == 0 ? NULL : files[loop_file - 1].path, &data_config);

									if (sound_data != NULL)
									{
										SoundDataListEntry *new_entry = (SoundDataListEntry*)malloc(sizeof(SoundDataListEntry));

										new_entry->sound_data = sound_data;
										new_entry->next = NULL;

										SoundDataListEntry **entry = &sound_data_list_head;
										while (*entry != NULL)
											entry = &(*entry)->next;

										*entry = new_entry;

										if (selected_sound_data == NULL)
											selected_sound_data = new_entry;
									}
								}
							ImGui::End();

							ImGui::Begin("Sound Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize);
								if (selected_sound_data == NULL)
								{
									ImGui::Text("No sound data selected...");
								}
								else
								{
									ImGui::Checkbox("Loop", &sound_config.loop);
									ImGui::Checkbox("Do not free when done", &sound_config.do_not_free_when_done);
									ImGui::Checkbox("Dynamic sample rate", &sound_config.dynamic_sample_rate);

									ImGui::Spacing();

									if (ImGui::Button("Create sound"))
									{
										ClownAudio_SoundID sound_id = ClownAudio_CreateSound(selected_sound_data->sound_data, &sound_config);

										if (sound_id != 0)
										{
											SoundListEntry *new_entry = (SoundListEntry*)malloc(sizeof(SoundListEntry));

											new_entry->sound_id = sound_id;
											new_entry->master_volume = 1.0f;
											new_entry->volume_left = 1.0f;
											new_entry->volume_right = 1.0f;
											new_entry->next = NULL;

											SoundListEntry **entry = &sound_list_head;
											while (*entry != NULL)
												entry = &(*entry)->next;

											*entry = new_entry;

											if (selected_sound == NULL)
												selected_sound = new_entry;
										}
									}
								}
							ImGui::End();

							ImGui::Begin("Sound Data", NULL);
								for (SoundDataListEntry *entry = sound_data_list_head; entry != NULL; entry = entry->next)
								{
									char name[32];
									sprintf(name, "Sound data %p", entry->sound_data);
									if (ImGui::Selectable(name, selected_sound_data == entry))
										selected_sound_data = entry;
								}

								if (selected_sound_data != NULL)
								{
									ImGui::Spacing();

									if (ImGui::Button("Destroy"))
									{
										ClownAudio_UnloadSoundData(selected_sound_data->sound_data);

										for (SoundDataListEntry **entry = &sound_data_list_head; *entry != NULL; entry = &(*entry)->next)
										{
											if (*entry == selected_sound_data)
											{
												SoundDataListEntry *next_sound_data = (*entry)->next;
												free(*entry);
												*entry = next_sound_data;
												selected_sound_data = *entry == NULL ? NULL : *entry;
												break;
											}
										}
									}
								}
							ImGui::End();

							ImGui::Begin("Sounds", NULL);
								for (SoundListEntry *entry = sound_list_head; entry != NULL; entry = entry->next)
								{
									int status = ClownAudio_GetSoundStatus(entry->sound_id);

									char name[32];

									switch (status)
									{
										case 0:
											sprintf(name, "Sound %u - Playing", entry->sound_id);
											break;

										case 1:
											sprintf(name, "Sound %u - Paused", entry->sound_id);
											break;

										case -1:
											sprintf(name, "Sound %u - Freed", entry->sound_id);
											break;

									}

									if (ImGui::Selectable(name, selected_sound == entry))
										selected_sound = entry;
								}

								if (selected_sound != NULL)
								{
									ImGui::Spacing();

									if (ImGui::Button("Destroy"))
									{
										ClownAudio_DestroySound(selected_sound->sound_id);

										for (SoundListEntry **entry = &sound_list_head; *entry != NULL; entry = &(*entry)->next)
										{
											if (*entry == selected_sound)
											{
												SoundListEntry *next_sound = (*entry)->next;
												free(*entry);
												*entry = next_sound;
												break;
											}
										}

										selected_sound = NULL;
									}
								}
							ImGui::End();

							ImGui::Begin("Sound Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);
								if (selected_sound == NULL)
								{
									ImGui::Text("No sound selected...");
								}
								else
								{
									int status = ClownAudio_GetSoundStatus(selected_sound->sound_id);

									if (status == -1)
									{
										ImGui::Text("Selected sound has been freed...");
									}
									else
									{
										if (status == 0)
										{
											if (ImGui::Button("Pause"))
												ClownAudio_PauseSound(selected_sound->sound_id);
										}
										else
										{
											if (ImGui::Button("Unpause"))
												ClownAudio_UnpauseSound(selected_sound->sound_id);
										}

										ImGui::SameLine();

										if (ImGui::Button("Rewind"))
											ClownAudio_RewindSound(selected_sound->sound_id);

										ImGui::Spacing();

										if (ImGui::Button("Fade-out"))
											ClownAudio_FadeOutSound(selected_sound->sound_id, 5 * 1000);

										ImGui::SameLine();

										if (ImGui::Button("Fade-in"))
											ClownAudio_FadeInSound(selected_sound->sound_id, 5 * 1000);

										ImGui::SameLine();

										if (ImGui::Button("Cancel fade"))
											ClownAudio_CancelFade(selected_sound->sound_id);

										ImGui::Spacing();

										if (ImGui::SliderFloat("Master volume", &selected_sound->master_volume, 0.0f, 1.0f, "%.3f", 2.0f))
											ClownAudio_SetSoundVolume(selected_sound->sound_id, selected_sound->master_volume * selected_sound->volume_left * 0x100, selected_sound->master_volume * selected_sound->volume_right * 0x100);

										if (ImGui::SliderFloat("Left volume", &selected_sound->volume_left, 0.0f, 1.0f, "%.3f", 2.0f))
											ClownAudio_SetSoundVolume(selected_sound->sound_id, selected_sound->master_volume * selected_sound->volume_left * 0x100, selected_sound->master_volume * selected_sound->volume_right * 0x100);

										if (ImGui::SliderFloat("Right volume", &selected_sound->volume_right, 0.0f, 1.0f, "%.3f", 2.0f))
											ClownAudio_SetSoundVolume(selected_sound->sound_id, selected_sound->master_volume * selected_sound->volume_left * 0x100, selected_sound->master_volume * selected_sound->volume_right * 0x100);

										ImGui::Spacing();

										static int sample_rate = 0;
										ImGui::InputInt("Sample rate", &sample_rate, 0);

										if (ImGui::Button("Apply sample rate"))
											ClownAudio_SetSoundSampleRate(selected_sound->sound_id, sample_rate, sample_rate);
									}
								}
							ImGui::End();

							ImGui::ShowDemoWindow();

							ImGui::Render();
							glClear(GL_COLOR_BUFFER_BIT);
							ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
							glfwSwapBuffers(window);
						}

						free(files);

						ClownAudio_Deinit();
					}
				}
			}
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	return 0;
}
