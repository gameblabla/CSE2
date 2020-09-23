#version 150 core
layout(location = 0) uniform sampler2D tex;
layout(location = 0) uniform vec4 colour;
in vec2 texture_coordinates;
out vec4 fragment;
void main()
{
	fragment = vec4(colour.rgb, colour.a * texture(tex, texture_coordinates).r);
}
