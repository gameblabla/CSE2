#version 150 core
layout(location = 0) in vec4 input_vertex_coordinates;
void main() 
{ 
	gl_Position = input_vertex_coordinates; 
} 
