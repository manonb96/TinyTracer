#version 450

#ifdef VULKAN
#include "common.glsl"
#endif

layout(location = 0) in vec2 vertex_uv;
layout(location = 0) out vec4 out_color;

#ifdef VULKAN
layout(set = 1, binding = 0) uniform sampler2D texture_sampler;
#else 
layout(binding = 0) uniform sampler2D texture_sampler;
#endif

void main()
{
	// texture is a build-in method
	out_color = texture(texture_sampler, vertex_uv);
} 