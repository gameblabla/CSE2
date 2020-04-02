#include <stddef.h>
#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 290

int main(int argc, char *argv[])
{
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

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DoConfig - Doukutsu Monogatari Settings", NULL, NULL);

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

					//////////////
					// Mainloop //
					//////////////

					while (!glfwWindowShouldClose(window))
					{
						glfwPollEvents();

						ImGui_ImplOpenGL3_NewFrame();
						ImGui_ImplGlfw_NewFrame();
						ImGui::NewFrame();

						ImGui::SetNextWindowPos(ImVec2(0, 0));
						ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));

						ImGui::Begin("Main window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

							if (ImGui::BeginTable("Block1", 2, ImGuiTableFlags_Borders))
							{
								ImGui::TableNextRow();

								ImGui::TableSetColumnIndex(0);
								static int movement = 0;
								ImGui::RadioButton("Arrows for Movement", &movement, 0);
								ImGui::RadioButton("<>? for Movement", &movement, 1);

								ImGui::TableSetColumnIndex(1);
								static int okay = 0;
								ImGui::RadioButton("Jump=Okay", &okay, 0);
								ImGui::RadioButton("Attack=Okay", &okay, 1);

								ImGui::TableNextRow();

								ImGui::TableSetColumnIndex(0);
								static int buttons = 0;
								ImGui::RadioButton("Z=Jump; X=Attack", &buttons, 0);
								ImGui::RadioButton("X=Jump; Z=Attack", &buttons, 1);

								ImGui::TableSetColumnIndex(1);
								ImGui::SetNextItemWidth(-1.0f);
								const char *items[] = {"Fullscreen 16-bit", "Windowed 320x240", "Windowed 640x480", "Fullscreen 24-bit", "Fullscreen 32-bit"};
								static int item_current = 0;
								ImGui::Combo("", &item_current, items, IM_ARRAYSIZE(items));
								static bool joypad;
								ImGui::Checkbox("Use Joypad", &joypad);

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
												const char *inputs[6] = {"Jump:   ", "Attack: ", "Weapon+:", "Weapon-:", "Items:  ", "Map:    "};
												ImGui::Text(inputs[y - 1]);
											}
											else
											{
												static char name_buffer[5] = {'#', '#', '0', '0', '\0'};
												static int button_bindings[8];
												name_buffer[2] = '0' + x;
												name_buffer[3] = '0' + y;
												ImGui::RadioButton(name_buffer, &button_bindings[x - 1], y - 1);
											}
										}
									}
								}

								ImGui::EndTable();

								ImGui::Button("Okay", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0.0f));
								ImGui::SameLine();
								ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
							}

						ImGui::End();

						ImGui::Render();
						glClear(GL_COLOR_BUFFER_BIT);
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
						glfwSwapBuffers(window);
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
