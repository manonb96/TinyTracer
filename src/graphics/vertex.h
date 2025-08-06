#pragma once

#include "../utils/precomp.h"
#include <vulkan/vulkan.h>

struct Vertex {
	Vertex() : position(float3(0.0f)), uv(float2(0.0f)) {}
	Vertex(float3 _position, float2 _uv) : position(_position), uv(_uv) {}

	float3 position;
	float2 uv;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription description = {};
		description.binding = 0;
		description.stride = sizeof(Vertex);
		description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return description;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> descriptions = {};
		descriptions[0].binding = 0;
		descriptions[0].location = 0;
		descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		descriptions[0].offset = offsetof(Vertex, position);

		descriptions[1].binding = 0;
		descriptions[1].location = 1;
		descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		descriptions[1].offset = offsetof(Vertex, uv);

		return descriptions;
	}
};