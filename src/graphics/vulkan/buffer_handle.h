#pragma once

#include <vulkan/vulkan.h>

struct BufferHandle {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	std::uint32_t element_count = 0;
};