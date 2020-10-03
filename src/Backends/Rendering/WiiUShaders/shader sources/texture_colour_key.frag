#version 150 core
layout(location = 0) uniform sampler2D tex;
in vec2 texture_coordinates;
out vec4 fragment;
void main()
{
	vec4 colour = texture(tex, texture_coordinates);

	if (colour.r + colour.g + colour. b == 0.0f)
		discard;

	fragment = colour;
}
