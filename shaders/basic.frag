#version 450

layout(location = 0) in vec2 vertex_uv;
layout(location = 0) out vec4 out_color;

uniform sampler2D texture_sampler;

void main()
{
	// texture is a build-in method
	out_color = texture(texture_sampler, vertex_uv);
} 