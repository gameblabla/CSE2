#version 150 core
layout(location = 0) uniform vec4 colour;
out vec4 fragment;
void main()
{
	fragment = colour;
}