#version 150 core
layout(location = 0) uniform vec2 vertex_transform;
layout(location = 1) uniform vec2 texture_coordinate_transform;
layout(location = 0) in vec4 input_vertex_coordinates;
layout(location = 1) in vec2 input_texture_coordinates;
out vec2 texture_coordinates; 
void main() 
{ 
	gl_Position = vec4(input_vertex_coordinates.x * vertex_transform.x - 1.0f, input_vertex_coordinates.y * vertex_transform.y + 1.0f, input_vertex_coordinates.zw);
	texture_coordinates = input_texture_coordinates * texture_coordinate_transform; 
} 
