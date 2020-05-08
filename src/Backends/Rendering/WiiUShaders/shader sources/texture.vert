#version 150 core
layout(location = 0) in vec4 input_vertex_coordinates;
layout(location = 1) in vec2 input_texture_coordinates;
out vec2 texture_coordinates; 
void main() 
{ 
	gl_Position = input_vertex_coordinates; 
	texture_coordinates = input_texture_coordinates; 
} 
