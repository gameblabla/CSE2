#version 150 core
layout(location = 0) uniform vec2 vertex_transform;
layout(location = 0) in vec4 input_vertex_coordinates;
void main() 
{ 
	gl_Position = vec4(input_vertex_coordinates.x * vertex_transform.x - 1.0f, input_vertex_coordinates.y * vertex_transform.y + 1.0f, input_vertex_coordinates.zw);
} 
