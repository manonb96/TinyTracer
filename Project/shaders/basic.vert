#version 450

#ifdef VULKAN
#include "common.glsl"
#endif

layout (location = 0) in vec3 input_position;
layout (location = 1) in vec2 input_uv;

layout (location = 0) out vec2 vertex_uv;

void main()
{
#ifdef VULKAN
	gl_Position = camera.projection * camera.view * vec4(input_position, 1.0);
#else
	gl_Position = vec4(input_position, 1.0);
	gl_Position.y *= -1;
#endif
	vertex_uv = input_uv;
}