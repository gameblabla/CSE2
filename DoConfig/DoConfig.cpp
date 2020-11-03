// Released under the MIT licence.
// See LICENCE.txt for details.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#ifdef LEGACY_OPENGL
#include "imgui/imgui_impl_opengl2.h"
#else
#include "imgui/imgui_impl_opengl3.h"
#endif

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 290

struct Config
{
	char proof[0x20];
	char font_name[0x40];
	int move_button_mode;
	int attack_button_mode;
	int ok_button_mode;
	int display_mode;
	bool bJoystick;
	int joystick_button[8];
};

static const char *proof = "DOUKUTSU20041206";

int main(int argc, char *argv[])
{
	(void)argc;

	char *config_path;

	for (size_t i = strlen(argv[0]);; --i)
	{
		if (i == 0 || argv[0][i - 1] == '\\' || argv[0][i - 1] == '/')
		{
			const char config_string[] = "Config.dat";

			config_path = (char*)malloc(i + sizeof(config_string));

			if (config_path == NULL)
				return 1;

			memcpy(config_path, argv[0], i);
			memcpy(config_path + i, config_string, sizeof(config_string));	// Will copy null-character

			break;
		}
	}

	/////////////////////
	// Initialise GLFW //
	/////////////////////

	if (glfwInit())
	{
	#ifdef LEGACY_OPENGL
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	#else
		const char *glsl_version = "#version 150 core";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	#endif

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DoConfig - Doukutsu Monogatari Settings", NULL, NULL);

		if (window != NULL)
		{
			glfwMakeContextCurrent(window);
			glfwSwapInterval(2);

			if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				// Check if the platform supports the version of OpenGL we want
			#ifdef LEGACY_OPENGL
				if (GLAD_GL_VERSION_2_1)
			#else
				if (GLAD_GL_VERSION_3_2)
			#endif
				{
					///////////////////////////
					// Initialise Dear ImGui //
					///////////////////////////

					IMGUI_CHECKVERSION();
					ImGui::CreateContext();
					ImGuiIO &io = ImGui::GetIO();
					io.IniFilename = NULL;	// Disable `imgui.ini`

					ImGui_ImplGlfw_InitForOpenGL(window, true);
				#ifdef LEGACY_OPENGL
					ImGui_ImplOpenGL2_Init();
				#else
					ImGui_ImplOpenGL3_Init(glsl_version);
				#endif

					/////////////////////
					// Load Config.dat //
					/////////////////////

					Config configuration;

					FILE *file = fopen(config_path, "rb");

					if (file != NULL)
					{
						// Read from file
						fread(configuration.proof, 1, sizeof(configuration.proof), file);
						fread(configuration.font_name, 1, sizeof(configuration.font_name), file);
						configuration.move_button_mode = fgetc(file);
						fseek(file, 3, SEEK_CUR);
						configuration.attack_button_mode = fgetc(file);
						fseek(file, 3, SEEK_CUR);
						configuration.ok_button_mode = fgetc(file);
						fseek(file, 3, SEEK_CUR);
						configuration.display_mode = fgetc(file);
						fseek(file, 3, SEEK_CUR);
						configuration.bJoystick = fgetc(file);

						for (unsigned int i = 0; i < 8; ++i)
						{
							const int decode_table[6] = {0, 1, 2, 4, 5, 3};

							fseek(file, 3, SEEK_CUR);
							configuration.joystick_button[i] = decode_table[fgetc(file) - 1];
						}

						fclose(file);
					}

					if (file == NULL || memcmp(configuration.proof, proof, strlen(proof)))
					{
						// Reset to defaults
						memset(&configuration, 0, sizeof(configuration));
						strcpy(configuration.proof, proof);
						strcpy(configuration.font_name, "Courier New");

						for (unsigned int i = 0; i < 8; ++i)
							configuration.joystick_button[i] = i % 6;
					}

					//////////////
					// Mainloop //
					//////////////

					while (!glfwWindowShouldClose(window))
					{
						glfwPollEvents();

					#ifdef LEGACY_OPENGL
						ImGui_ImplOpenGL2_NewFrame();
					#else
						ImGui_ImplOpenGL3_NewFrame();
					#endif
						ImGui_ImplGlfw_NewFrame();
						ImGui::NewFrame();

						ImGui::SetNextWindowPos(ImVec2(0, 0));
						ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));

						ImGui::Begin("Main window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

							if (ImGui::BeginTable("Block1", 2, ImGuiTableFlags_Borders))
							{
								ImGui::TableNextRow();

								ImGui::TableSetColumnIndex(0);
								ImGui::RadioButton("Arrows for Movement", &configuration.move_button_mode, 0);
								ImGui::RadioButton("<>? for Movement", &configuration.move_button_mode, 1);

								ImGui::TableSetColumnIndex(1);
								ImGui::RadioButton("Jump=Okay", &configuration.ok_button_mode, 0);
								ImGui::RadioButton("Attack=Okay", &configuration.ok_button_mode, 1);

								ImGui::TableNextRow();

								ImGui::TableSetColumnIndex(0);
								ImGui::RadioButton("Z=Jump; X=Attack", &configuration.attack_button_mode, 0);
								ImGui::RadioButton("X=Jump; Z=Attack", &configuration.attack_button_mode, 1);

								ImGui::TableSetColumnIndex(1);
								ImGui::SetNextItemWidth(-1.0f);
								const char *items[] = {"Fullscreen 16-bit", "Windowed 320x240", "Windowed 640x480", "Fullscreen 24-bit", "Fullscreen 32-bit"};
								ImGui::Combo("", &configuration.display_mode, items, IM_ARRAYSIZE(items));
								ImGui::Checkbox("Use Joypad", &configuration.bJoystick);

								ImGui::EndTable();
							}

							// Joypad binding

							if (ImGui::BeginTable("Joypad binding", 9))
							{
								ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 60.0f);

								for (int y = 0; y < 7; ++y)
								{
									ImGui::TableNextRow();

									if (y == 0)
									{
										for (int x = 1; x < 9; ++x)
										{
											ImGui::TableSetColumnIndex(x);
											ImGui::Text(" %d", x);
										}
									}
									else
									{
										for (int x = 0; x < 9; ++x)
										{
											ImGui::TableSetColumnIndex(x);

											if (x == 0)
											{
												const char *inputs[6] = {"Jump:", "Attack:", "Weapon+:", "Weapon-:", "Items:", "Map:"};
												ImGui::Text("%s", inputs[y - 1]);
											}
											else
											{
												static char name_buffer[5] = {'#', '#', '0', '0', '\0'};
												name_buffer[2] = '0' + x;
												name_buffer[3] = '0' + y;
												ImGui::RadioButton(name_buffer, &configuration.joystick_button[x - 1], y - 1);
											}
										}
									}
								}

								ImGui::EndTable();

								if (ImGui::Button("Okay", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f)))
								{
									glfwSetWindowShouldClose(window, 1);

									// Save to file
									FILE *file = fopen(config_path, "wb");

									if (file != NULL)
									{
										fwrite(configuration.proof, 1, sizeof(configuration.proof), file);
										fwrite(configuration.font_name, 1, sizeof(configuration.font_name), file);
										fputc(configuration.move_button_mode, file);
										fputc(0, file);
										fputc(0, file);
										fputc(0, file);
										fputc(configuration.attack_button_mode, file);
										fputc(0, file);
										fputc(0, file);
										fputc(0, file);
										fputc(configuration.ok_button_mode, file);
										fputc(0, file);
										fputc(0, file);
										fputc(0, file);
										fputc(configuration.display_mode, file);
										fputc(0, file);
										fputc(0, file);
										fputc(0, file);
										fputc(configuration.bJoystick, file);
										fputc(0, file);
										fputc(0, file);
										fputc(0, file);

										for (unsigned int i = 0; i < 8; ++i)
										{
											const int encode_table[6] = {0, 1, 2, 5, 3, 4};

											fputc(encode_table[configuration.joystick_button[i]] + 1, file);
											fputc(0, file);
											fputc(0, file);
											fputc(0, file);
										}

										fclose(file);
									}
								}

								ImGui::SameLine();

								if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
									glfwSetWindowShouldClose(window, 1);
							}

						ImGui::End();

						ImGui::Render();
						glClear(GL_COLOR_BUFFER_BIT);
					#ifdef LEGACY_OPENGL
						ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
					#else
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					#endif
						glfwSwapBuffers(window);
					}
				}
			}
		}

	#ifdef LEGACY_OPENGL
		ImGui_ImplOpenGL2_Shutdown();
	#else
		ImGui_ImplOpenGL3_Shutdown();
	#endif
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	free(config_path);

	return 0;
}
