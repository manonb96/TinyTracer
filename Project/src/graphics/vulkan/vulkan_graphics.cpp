#define GLFW_INCLUDE_VULKAN
#include <set>
#include "vulkan_graphics.hpp"

#pragma region Vulkan functions extension implementations

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) {
	PFN_vkCreateDebugUtilsMessengerEXT function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (function != nullptr) {
		return function(instance, info, allocator, debug_messenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator) {
	PFN_vkDestroyDebugUtilsMessengerEXT function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (function != nullptr) {
		function(instance, debug_messenger, allocator);
	}
}

#pragma endregion

#pragma region Validation Layers

static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		spdlog::warn("Vulkan Validation: {}", callback_data->pMessage);
	}
	else {
		spdlog::error("Vulkan Error: {}", callback_data->pMessage);
	}
	return VK_FALSE;
}

static VkDebugUtilsMessengerCreateInfoEXT GetCreateMessengerInfo() {
	VkDebugUtilsMessengerCreateInfoEXT creation_info = {};
	creation_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	creation_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	creation_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	creation_info.pfnUserCallback = ValidationCallback;
	creation_info.pUserData = nullptr;
	return creation_info;
}

void VulkanGraphics::SetupDebugMessenger() {
	if (!m_validationEnabled) {
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT info = GetCreateMessengerInfo();
	VkResult result = vkCreateDebugUtilsMessengerEXT(m_instance, &info, nullptr, &m_debugMessenger);
	if (result != VK_SUCCESS) {
		spdlog::error("Cannot create debug messenger");
		return;
	}
}

bool LayerMatchesName(cstring name, const VkLayerProperties& properties) {
	return IsStringEqual(properties.layerName, name);
}

bool IsLayerSupported(vector<VkLayerProperties> layers, cstring name) {
	return std::any_of(layers.begin(), layers.end(), std::bind_front(LayerMatchesName, name));
}

bool VulkanGraphics::AreAllLayersSupported(span<cstring> layers) {
	vector<VkLayerProperties> supported_layers = GetSupportedValidationLayers();
	return std::all_of(layers.begin(), layers.end(), std::bind_front(IsLayerSupported, supported_layers));
}

vector<VkLayerProperties> VulkanGraphics::GetSupportedValidationLayers() {
	uint count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	if (count == 0) {
		return {};
	}

	vector<VkLayerProperties> properties(count);
	vkEnumerateInstanceLayerProperties(&count, properties.data());
	return properties;
}

#pragma endregion

#pragma region Instance and extensions

void VulkanGraphics::CreateInstance() {
	std::array<cstring, 1> validation_layers = { "VK_LAYER_KHRONOS_validation" };
	if (!AreAllLayersSupported(validation_layers)) {
		m_validationEnabled = false;
	}

	vector<cstring> required_extensions = GetRequiredInstanceExtensions();

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr; 
	app_info.pApplicationName = "Vulkan application";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "VEng";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instance_creation_info = {};
	instance_creation_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_creation_info.pNext = nullptr;
	instance_creation_info.pApplicationInfo = &app_info;
	instance_creation_info.enabledExtensionCount = required_extensions.size();
	instance_creation_info.ppEnabledExtensionNames = required_extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT messenger_creation_info = GetCreateMessengerInfo();

	if (m_validationEnabled) {
		instance_creation_info.pNext = &messenger_creation_info;
		instance_creation_info.enabledLayerCount = validation_layers.size();
		instance_creation_info.ppEnabledLayerNames = validation_layers.data();
	}
	else {
		instance_creation_info.enabledLayerCount = 0;
		instance_creation_info.ppEnabledLayerNames = nullptr;
	}

	VkResult result = vkCreateInstance(&instance_creation_info, nullptr, &m_instance);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

span<cstring> VulkanGraphics::GetSuggestedInstanceExtensions() {
	uint glfw_extension_count = 0;
	cstring* glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	return { glfw_extensions, glfw_extension_count };
}

vector<cstring> VulkanGraphics::GetRequiredInstanceExtensions() {
	span<cstring> suggested_extensions = GetSuggestedInstanceExtensions();
	vector<cstring> required_extensions(suggested_extensions.size());
	std::copy(suggested_extensions.begin(), suggested_extensions.end(), required_extensions.begin());

	if (m_validationEnabled) {
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!AreAllExtensionsSupported(required_extensions)) {
		std::exit(EXIT_FAILURE);
	}

	return required_extensions;
}

vector<VkExtensionProperties> VulkanGraphics::GetSupportedInstanceExtensions() {
	uint count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	if (count == 0) {
		return {};
	}

	vector<VkExtensionProperties> properties(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
	return properties;
}

bool ExtensionMatchesName(cstring name, const VkExtensionProperties& properties) {
	return IsStringEqual(properties.extensionName, name);
}

bool IsExtensionSupported(vector<VkExtensionProperties> supportedextensions, cstring name) {
	return std::any_of(supportedextensions.begin(), supportedextensions.end(), std::bind_front(ExtensionMatchesName, name));
}

bool VulkanGraphics::AreAllExtensionsSupported(span<cstring> extensions) {
	vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();
	return std::all_of(extensions.begin(), extensions.end(), std::bind_front(IsExtensionSupported, supported_extensions));
}

#pragma endregion

#pragma region Devices and queues
VulkanGraphics::QueueFamilyIndices VulkanGraphics::FindQueueFamilies(VkPhysicalDevice device) {
	uint queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
	vector<VkQueueFamilyProperties> families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

	auto graphics_family_it = std::find_if(families.begin(), families.end(), [](const VkQueueFamilyProperties& props) {
		return props.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);
		});

	QueueFamilyIndices result;
	result.graphics_family = graphics_family_it - families.begin();

	for (uint i = 0; i < families.size(); i++) {
		VkBool32 has_presentation_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &has_presentation_support);
		if (has_presentation_support) {
			result.presentation_family = i;  // It might be the same as Graphics Family, but consider separately
			break;
		}
	}

	return result;
}

VulkanGraphics::SwapChainProperties VulkanGraphics::GetSwapChainProperties(VkPhysicalDevice device) {
	SwapChainProperties properties;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &properties.capabilities);

	uint format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);
	properties.formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, properties.formats.data());

	uint modes_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &modes_count, nullptr);
	properties.present_modes.resize(modes_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &modes_count, properties.present_modes.data());

	return properties;
}

vector<VkExtensionProperties> VulkanGraphics::GetDeviceAvailableExtensions(VkPhysicalDevice device) {
	uint available_extensions_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extensions_count, nullptr);
	vector<VkExtensionProperties> available_extensions(available_extensions_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extensions_count, available_extensions.data());

	return available_extensions;
}

bool VulkanGraphics::AreAllDeviceExtensionsSupported(VkPhysicalDevice device) {
	vector<VkExtensionProperties> available_extensions = GetDeviceAvailableExtensions(device);
	return std::all_of(m_requiredDeviceExtensions.begin(), m_requiredDeviceExtensions.end(), std::bind_front(IsExtensionSupported, available_extensions));
}

bool VulkanGraphics::IsDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices families = FindQueueFamilies(device);
	return families.IsValid() && AreAllDeviceExtensionsSupported(device) &&
		GetSwapChainProperties(device).IsValid(); 
}

void VulkanGraphics::PickPhysicalDevice() {
	vector<VkPhysicalDevice> devices = GetAvailableDevices();
	std::erase_if(devices, std::not_fn(std::bind_front(&VulkanGraphics::IsDeviceSuitable, this)));

	if (devices.empty()) {
		spdlog::error("No physical devices that match the criteria");
		std::exit(EXIT_FAILURE);
	}

	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(devices[0], &device_properties);
	spdlog::info("Setting {} as physical device (total # physical devices available: {})", device_properties.deviceName, devices.size());
	
	// We just pick the first available device
	m_physicalDevice = devices[0];
}

vector<VkPhysicalDevice> VulkanGraphics::GetAvailableDevices() {
	uint device_count;
	vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

	if (device_count == 0) {
		return {};
	}

	vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

	return devices;
}

void VulkanGraphics::CreateLogicalDeviceAndQueues() {
	QueueFamilyIndices picked_device_families = FindQueueFamilies(m_physicalDevice);

	// Sanity check
	if (!picked_device_families.IsValid()) {
		std::exit(EXIT_FAILURE);
	}

	// If graphics family and presentation family at the same, the set will only have one value
	std::set<uint> unique_queue_families = { picked_device_families.graphics_family.value(), picked_device_families.presentation_family.value() };

	float queue_priority = 1.0f;

	vector<VkDeviceQueueCreateInfo> queue_create_infos;
	for (uint unique_queue_family : unique_queue_families) {
		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = unique_queue_family;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_info);
	}

	VkPhysicalDeviceFeatures required_features = {};
	required_features.depthBounds = true; // Has to be true if you want to enable it later
	required_features.depthClamp = true;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = queue_create_infos.size();
	device_info.pQueueCreateInfos = queue_create_infos.data();
	device_info.pEnabledFeatures = &required_features;
	device_info.enabledExtensionCount = m_requiredDeviceExtensions.size();
	device_info.ppEnabledExtensionNames = m_requiredDeviceExtensions.data();
	device_info.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(m_physicalDevice, &device_info, nullptr, &m_logicalDevice);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	vkGetDeviceQueue(m_logicalDevice, picked_device_families.graphics_family.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logicalDevice, picked_device_families.presentation_family.value(), 0, &m_presentQueue);
}

#pragma endregion

#pragma region Presentation

void VulkanGraphics::CreateSurface() {
	VkResult result = glfwCreateWindowSurface(m_instance, m_pWindow->GetHandle(), nullptr, &m_surface);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

bool IsRgbaTypeFormat(const VkSurfaceFormatKHR& format_properties) {
	return format_properties.format == VK_FORMAT_R8G8B8A8_SRGB || format_properties.format == VK_FORMAT_B8G8R8A8_SRGB;
}

bool IsSrgbColorSpace(const VkSurfaceFormatKHR& format_properties) {
	return format_properties.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
}

bool IsCorrectFormat(const VkSurfaceFormatKHR& format_properties) {
	return IsRgbaTypeFormat(format_properties) && IsSrgbColorSpace(format_properties);
}

VkSurfaceFormatKHR VulkanGraphics::ChooseSwapSurfaceFormat(span<VkSurfaceFormatKHR> formats) {
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	auto it = std::find_if(formats.begin(), formats.end(), IsCorrectFormat);
	if (it != formats.end()) {
		return *it;
	}

	return formats[0];
}

// Mailbox present mode: One frame rendering, one in queue
bool IsMailboxPresentMode(const VkPresentModeKHR& mode) {
	return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

VkPresentModeKHR VulkanGraphics::ChooseSwapPresentMode(span<VkPresentModeKHR> modes) {
	if (std::any_of(modes.begin(), modes.end(), IsMailboxPresentMode)) {
		return VK_PRESENT_MODE_MAILBOX_KHR;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanGraphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	constexpr uint kInvalidSize = std::numeric_limits<uint>::max();
	if (capabilities.currentExtent.width != kInvalidSize) {
		return capabilities.currentExtent;
	}
	else {
		int2 size = m_pWindow->GetFrameBufferSize();
		VkExtent2D actual_extent = {
			static_cast<uint>(size.x),
			static_cast<uint>(size.y),
		};
		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual_extent;
	}
}

uint VulkanGraphics::ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
	uint image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < image_count) {
		image_count = capabilities.maxImageCount;
	}
	return image_count;
}

void VulkanGraphics::CreateSwapChain() {
	SwapChainProperties properties = GetSwapChainProperties(m_physicalDevice);

	m_surfaceFormat = ChooseSwapSurfaceFormat(properties.formats);
	m_presentMode = ChooseSwapPresentMode(properties.present_modes);
	m_extent = ChooseSwapExtent(properties.capabilities);
	uint image_count = ChooseSwapImageCount(properties.capabilities);

	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; 
	info.surface = m_surface;
	info.minImageCount = image_count;
	info.imageFormat = m_surfaceFormat.format;
	info.imageColorSpace = m_surfaceFormat.colorSpace;
	info.imageExtent = m_extent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.presentMode = m_presentMode;
	info.preTransform = properties.capabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.clipped = VK_TRUE;
	info.oldSwapchain = VK_NULL_HANDLE;

	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

	if (indices.graphics_family != indices.presentation_family) {
		std::array<uint, 2> family_indices = { indices.graphics_family.value(), indices.presentation_family.value() };
		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = family_indices.size();
		info.pQueueFamilyIndices = family_indices.data();
	}
	else {
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VkResult result = vkCreateSwapchainKHR(m_logicalDevice, &info, nullptr, &m_swapChain);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	uint actual_image_count;
	vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &actual_image_count, nullptr);
	m_swapChainImages.resize(actual_image_count);
	vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &actual_image_count, m_swapChainImages.data());
}

void VulkanGraphics::RecreateSwapChain() {
	int2 size = m_pWindow->GetFrameBufferSize();
	while (size.x == 0 || size.y == 0) {
		size = m_pWindow->GetFrameBufferSize();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_logicalDevice);
	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateFramebuffers();
}

void VulkanGraphics::CleanupSwapChain() {
	if (m_logicalDevice == VK_NULL_HANDLE) {
		return;
	}

	for (VkFramebuffer framebuffer : m_swapChainFrameBuffers) {
		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
		}
	}

	for (VkImageView image_view : m_swapChainImageViews) {
		if (image_view != VK_NULL_HANDLE) {
			vkDestroyImageView(m_logicalDevice, image_view, nullptr);
		}
	}

	if (m_swapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
	}
}

VkImageView VulkanGraphics::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flag) {
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.image = image;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.subresourceRange.aspectMask = aspect_flag;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	VkImageView view;
	VkResult result = vkCreateImageView(m_logicalDevice, &info, nullptr, &view);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	return view;
}

void VulkanGraphics::CreateImageViews() {
	m_swapChainImageViews.resize(m_swapChainImages.size());
	auto image_view_it = m_swapChainImageViews.begin();
	for (VkImage image : m_swapChainImages) {
		*image_view_it = CreateImageView(image, m_surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		image_view_it = std::next(image_view_it);
	}
}

#pragma endregion

#pragma region Graphics Pipeline

VkShaderModule VulkanGraphics::CreateShaderModule(span<uchar> buffer) {
	if (buffer.empty()) {
		spdlog::error("[Vulkan Error] Could not create shader module. Found empty buffer.");
		return VK_NULL_HANDLE;
	}

	VkShaderModuleCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = buffer.size();
	info.pCode = reinterpret_cast<uint*>(buffer.data());

	VkShaderModule shader_module;
	VkResult result = vkCreateShaderModule(m_logicalDevice, &info, nullptr, &shader_module);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	return shader_module;
}

void VulkanGraphics::CreateGraphicsPipeline() {
	vector<uchar> basic_vertex_data = m_pShader->GetVertexShaderBytes();
	VkShaderModule vertex_shader = CreateShaderModule(basic_vertex_data);
	gsl::final_action _destroy_vertex([this, vertex_shader]() {
		vkDestroyShaderModule(m_logicalDevice, vertex_shader, nullptr);
		});

	vector<uchar> basic_fragment_data = m_pShader->GetFragmentShaderBytes();
	VkShaderModule fragment_shader = CreateShaderModule(basic_fragment_data);
	gsl::final_action _destroy_fragment([this, fragment_shader]() {
		vkDestroyShaderModule(m_logicalDevice, fragment_shader, nullptr);
		});

	if (vertex_shader == VK_NULL_HANDLE || fragment_shader == VK_NULL_HANDLE) {
		std::exit(EXIT_FAILURE);
	}

	VkPipelineShaderStageCreateInfo vertex_stage_info = {};
	vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_stage_info.module = vertex_shader;
	vertex_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo fragment_stage_info = {};
	fragment_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_stage_info.module = fragment_shader;
	fragment_stage_info.pName = "main";

	std::array<VkPipelineShaderStageCreateInfo, 2> stage_infos = { vertex_stage_info, fragment_stage_info };

	std::array<VkDynamicState, 2> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = dynamic_states.size();
	dynamic_state_info.pDynamicStates = dynamic_states.data();

	VkViewport viewport = GetViewport();
	VkRect2D scissor = GetScissor();

	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;

	auto vertex_binding_description = Vertex::GetBindingDescription();
	auto vertex_attribute_descriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.pVertexBindingDescriptions = &vertex_binding_description;
	vertex_input_info.vertexAttributeDescriptionCount = vertex_attribute_descriptions.size();
	vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterization_state_info = {};
	rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_info.depthClampEnable = VK_FALSE;
	rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_info.lineWidth = 1.0f;
	rasterization_state_info.cullMode = VK_CULL_MODE_NONE;
	rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_info.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling_info = {};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blending_info = {};
	color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending_info.logicOpEnable = VK_FALSE;
	color_blending_info.attachmentCount = 1;
	color_blending_info.pAttachments = &color_blend_attachment;

	VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
	depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_info.depthBoundsTestEnable = VK_TRUE;
	depth_stencil_info.minDepthBounds = 0.0f;
	depth_stencil_info.maxDepthBounds = 1.0f;
	depth_stencil_info.stencilTestEnable = VK_FALSE;

	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	 VkPushConstantRange model_matrix_range = {};
	 model_matrix_range.offset = 0;
	 model_matrix_range.size = sizeof(mat4);
	 model_matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	 layout_info.pushConstantRangeCount = 1;
	 layout_info.pPushConstantRanges = &model_matrix_range;

	std::array<VkDescriptorSetLayout, 2> set_layouts = { m_uniformSetLayout, m_textureSetLayout };
	layout_info.setLayoutCount = set_layouts.size();
	layout_info.pSetLayouts = set_layouts.data();

	VkResult layout_result = vkCreatePipelineLayout(m_logicalDevice, &layout_info, nullptr, &m_pipelineLayout);
	if (layout_result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = stage_infos.size();
	pipeline_info.pStages = stage_infos.data();
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterization_state_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = &depth_stencil_info;
	pipeline_info.pColorBlendState = &color_blending_info;
	pipeline_info.pDynamicState = &dynamic_state_info;
	pipeline_info.layout = m_pipelineLayout;
	pipeline_info.renderPass = m_renderPass;
	pipeline_info.subpass = 0;

	VkResult pipeline_result = vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline);
	if (pipeline_result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

VkViewport VulkanGraphics::GetViewport() {
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_extent.width);
	viewport.height = static_cast<float>(m_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	return viewport;
}

VkRect2D VulkanGraphics::GetScissor() {
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_extent;

	return scissor;
}

void VulkanGraphics::CreateRenderPass() {
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = m_surfaceFormat.format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.format = VK_FORMAT_D32_SFLOAT;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription main_subpass = {};
	main_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	main_subpass.colorAttachmentCount = 1;
	main_subpass.pColorAttachments = &color_attachment_ref;
	main_subpass.pDepthStencilAttachment = nullptr;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	
	std::array<VkAttachmentDescription, 1> attachments = { color_attachment };

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = attachments.size();
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &main_subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(m_logicalDevice, &render_pass_info, nullptr, &m_renderPass);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

#pragma endregion

#pragma region Drawing

void VulkanGraphics::CreateFramebuffers() {
	m_swapChainFrameBuffers.resize(m_swapChainImageViews.size());
	for (uint i = 0; i < m_swapChainImageViews.size(); i++) {
		std::array<VkImageView, 1> attachments = { m_swapChainImageViews[i] };

		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_renderPass;
		info.attachmentCount = attachments.size();
		info.pAttachments = attachments.data();
		info.width = m_extent.width;
		info.height = m_extent.height;
		info.layers = 1;

		VkResult result = vkCreateFramebuffer(m_logicalDevice, &info, nullptr, &m_swapChainFrameBuffers[i]);
		if (result != VK_SUCCESS) {
			std::exit(EXIT_FAILURE);
		}
	}
}

void VulkanGraphics::CreateCommandPool() {
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = indices.graphics_family.value();

	VkResult result = vkCreateCommandPool(m_logicalDevice, &pool_info, nullptr, &m_commandPool);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

void VulkanGraphics::CreateCommandBuffer() {
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandPool = m_commandPool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(m_logicalDevice, &info, &m_commandBuffer);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

void VulkanGraphics::BeginCommands() {
	vkResetCommandBuffer(m_commandBuffer, 0);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VkResult begin_state = vkBeginCommandBuffer(m_commandBuffer, &begin_info);
	if (begin_state != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin command buffer!");
	}

	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = m_renderPass;

	render_pass_begin_info.framebuffer = m_swapChainFrameBuffers[m_currentImageIndex];
	render_pass_begin_info.renderArea.offset = { 0, 0 };
	render_pass_begin_info.renderArea.extent = m_extent;

	std::array<VkClearValue, 2> clear_values;
	clear_values[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clear_values[1].depthStencil = { 1.0f, 0 }; 
	render_pass_begin_info.clearValueCount = clear_values.size();
	render_pass_begin_info.pClearValues = clear_values.data();
	vkCmdBeginRenderPass(m_commandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
	VkViewport viewport = GetViewport();
	VkRect2D scissor = GetScissor();

	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void VulkanGraphics::EndCommands() {
	vkCmdEndRenderPass(m_commandBuffer);
	VkResult end_buffer_result = vkEndCommandBuffer(m_commandBuffer);
	if (end_buffer_result != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}

void VulkanGraphics::CreateSignals() {
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_logicalDevice, &semaphore_info, nullptr, &m_imageAvailableSignal) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	m_renderFinishedSignals.resize(m_swapChainImageViews.size());
	for (uint i = 0; i < m_swapChainImageViews.size(); i++) {
		if (vkCreateSemaphore(m_logicalDevice, &semaphore_info, nullptr, &m_renderFinishedSignals[i]) != VK_SUCCESS) {
			std::exit(EXIT_FAILURE);
		}
	}

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(m_logicalDevice, &fence_info, nullptr, &m_stillRenderingFence) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

bool VulkanGraphics::BeginFrame() {
	vkWaitForFences(m_logicalDevice, 1, &m_stillRenderingFence, VK_TRUE, UINT64_MAX);
	VkResult image_acquire_result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, UINT64_MAX, m_imageAvailableSignal, VK_NULL_HANDLE, &m_currentImageIndex);

	if (image_acquire_result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		RecreateSwapChain();
		return false;
	}

	if (image_acquire_result != VK_SUCCESS && image_acquire_result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Couldn't acquire render image!");
	}

	vkResetFences(m_logicalDevice, 1, &m_stillRenderingFence);
	BeginCommands();
	return true;
}

void VulkanGraphics::EndFrame() {
	EndCommands();

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &m_imageAvailableSignal;
	submit_info.pWaitDstStageMask = &wait_stage;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_commandBuffer;

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &m_renderFinishedSignals[m_currentImageIndex];

	VkResult submit_result = vkQueueSubmit(m_graphicsQueue, 1, &submit_info, m_stillRenderingFence);
	if (submit_result != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw commands!");
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &m_renderFinishedSignals[m_currentImageIndex];
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &m_swapChain;
	present_info.pImageIndices = &m_currentImageIndex;

	VkResult result = vkQueuePresentKHR(m_presentQueue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image!");
	}
}
#pragma endregion

#pragma region Buffers

uint VulkanGraphics::FindMemoryType(uint type_bits_filter, VkMemoryPropertyFlags required_properties) {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memory_properties);
	span<VkMemoryType> memory_types(memory_properties.memoryTypes, memory_properties.memoryTypeCount);

	for (uint i = 0; i < memory_types.size(); i++) {
		bool passes_filter = type_bits_filter & (1 << i);
		bool has_property_flags = memory_types[i].propertyFlags & required_properties;
		if (passes_filter && has_property_flags) {
			return i;
		}
	}

	throw std::runtime_error("Cannot find memory type!");
}

BufferHandle VulkanGraphics::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, uint element_count) {
	BufferHandle handle = {};
	handle.element_count = element_count;

	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(m_logicalDevice, &buffer_info, nullptr, &handle.buffer);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(m_logicalDevice, handle.buffer, &memory_requirements);

	uint chosen_memory_type = FindMemoryType(memory_requirements.memoryTypeBits, properties);

	VkMemoryAllocateInfo allocation_info = {};
	allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocation_info.allocationSize = memory_requirements.size;
	allocation_info.memoryTypeIndex = chosen_memory_type;

	VkResult allocation_result = vkAllocateMemory(m_logicalDevice, &allocation_info, nullptr, &handle.memory);
	if (allocation_result != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_logicalDevice, handle.buffer, handle.memory, 0);
	return handle;
}

void VulkanGraphics::CreateVertexBuffer(span<Vertex> vertices) {
	VkDeviceSize size = sizeof(Vertex) * vertices.size();
	BufferHandle staging_handle = CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.size());

	void* data;
	vkMapMemory(m_logicalDevice, staging_handle.memory, 0, size, 0, &data);
	std::memcpy(data, vertices.data(), size);
	vkUnmapMemory(m_logicalDevice, staging_handle.memory);

	m_vertexBuffer = CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.size());

	VkCommandBuffer transient_commands = BeginTransientCommandBuffer();

	VkBufferCopy copy_info = {};
	copy_info.srcOffset = 0;
	copy_info.dstOffset = 0;
	copy_info.size = size;
	vkCmdCopyBuffer(transient_commands, staging_handle.buffer, m_vertexBuffer.buffer, 1, &copy_info);

	EndTransientCommandBuffer(transient_commands);
	DestroyBuffer(staging_handle);
}

void VulkanGraphics::CreateIndexBuffer(span<int> indices) {
	VkDeviceSize size = sizeof(uint) * indices.size();
	BufferHandle staging_handle = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.size());

	void* data;
	vkMapMemory(m_logicalDevice, staging_handle.memory, 0, size, 0, &data);
	std::memcpy(data, indices.data(), size);
	vkUnmapMemory(m_logicalDevice, staging_handle.memory);

	m_indexBuffer = CreateBuffer(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.size());

	VkCommandBuffer transient_commands = BeginTransientCommandBuffer();

	VkBufferCopy copy_info = {};
	copy_info.srcOffset = 0;
	copy_info.dstOffset = 0;
	copy_info.size = size;
	vkCmdCopyBuffer(transient_commands, staging_handle.buffer, m_indexBuffer.buffer, 1, &copy_info);

	EndTransientCommandBuffer(transient_commands);
	DestroyBuffer(staging_handle);
}

void VulkanGraphics::DestroyBuffer(BufferHandle handle) {
	vkDeviceWaitIdle(m_logicalDevice);
	vkDestroyBuffer(m_logicalDevice, handle.buffer, nullptr);
	vkFreeMemory(m_logicalDevice, handle.memory, nullptr);
}

void VulkanGraphics::RenderIndexedBuffer() {
	VkDeviceSize offset = 0;
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_uniformSet, 0, nullptr);
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 1, 1, &m_textureHandle.set, 0, nullptr);
	vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &m_vertexBuffer.buffer, &offset);
	vkCmdBindIndexBuffer(m_commandBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(m_commandBuffer, m_indexBuffer.element_count, 1, 0, 0, 0);
}

void VulkanGraphics::SetViewProjection(mat4 view, mat4 projection) {
	UniformTransformations transformations{ view, projection };
	std::memcpy(m_pUniformBufferLocation, &transformations, sizeof(UniformTransformations));
}

VkCommandBuffer VulkanGraphics::BeginTransientCommandBuffer() {
	VkCommandBufferAllocateInfo allocation_info = {};
	allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocation_info.commandPool = m_commandPool;
	allocation_info.commandBufferCount = 1;

	VkCommandBuffer buffer;
	vkAllocateCommandBuffers(m_logicalDevice, &allocation_info, &buffer);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(buffer, &begin_info);

	return buffer;
}

void VulkanGraphics::EndTransientCommandBuffer(VkCommandBuffer command_buffer) {
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);
	vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &command_buffer);
}

void VulkanGraphics::CreateUniformBuffers() {
	VkDeviceSize buffer_size = sizeof(UniformTransformations);
	m_uniformBuffer = CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 1);
	vkMapMemory(m_logicalDevice, m_uniformBuffer.memory, 0, buffer_size, 0, &m_pUniformBufferLocation);
}

void VulkanGraphics::CreateDescriptorSetLayouts() {
	VkDescriptorSetLayoutBinding uniform_layout_binding = {};
	uniform_layout_binding.binding = 0;
	uniform_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_layout_binding.descriptorCount = 1;
	uniform_layout_binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

	VkDescriptorSetLayoutCreateInfo uniform_layout_info = {};
	uniform_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	uniform_layout_info.bindingCount = 1;
	uniform_layout_info.pBindings = &uniform_layout_binding;

	if (vkCreateDescriptorSetLayout(m_logicalDevice, &uniform_layout_info, nullptr, &m_uniformSetLayout) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	VkDescriptorSetLayoutBinding texture_layout_binding = {};
	texture_layout_binding.binding = 0;
	texture_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	texture_layout_binding.descriptorCount = 1;
	texture_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo texture_layout_info = {};
	texture_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	texture_layout_info.bindingCount = 1;
	texture_layout_info.pBindings = &texture_layout_binding;

	if (vkCreateDescriptorSetLayout(m_logicalDevice, &texture_layout_info, nullptr, &m_textureSetLayout) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

void VulkanGraphics::CreateDescriptorPools() {
	VkDescriptorPoolSize uniform_pool_size = {};
	uniform_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_pool_size.descriptorCount = 1;

	VkDescriptorPoolCreateInfo uniform_pool_info = {};
	uniform_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	uniform_pool_info.poolSizeCount = 1;
	uniform_pool_info.pPoolSizes = &uniform_pool_size;
	uniform_pool_info.maxSets = 1;

	if (vkCreateDescriptorPool(m_logicalDevice, &uniform_pool_info, nullptr, &m_uniformPool) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	VkPhysicalDeviceProperties properties = {};
	vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

	VkDescriptorPoolSize texture_pool_size = {};
	texture_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	texture_pool_size.descriptorCount = properties.limits.maxSamplerAllocationCount;

	VkDescriptorPoolCreateInfo texture_pool_info = {};
	texture_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	texture_pool_info.poolSizeCount = 1;
	texture_pool_info.pPoolSizes = &texture_pool_size;
	texture_pool_info.maxSets = properties.limits.maxSamplerAllocationCount;
	texture_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; 

	if (vkCreateDescriptorPool(m_logicalDevice, &texture_pool_info, nullptr, &m_texturePool) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

void VulkanGraphics::CreateDescriptorSets() {
	VkDescriptorSetAllocateInfo set_info = {};
	set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_info.descriptorPool = m_uniformPool;
	set_info.descriptorSetCount = 1;
	set_info.pSetLayouts = &m_uniformSetLayout;

	VkResult result = vkAllocateDescriptorSets(m_logicalDevice, &set_info, &m_uniformSet);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	VkDescriptorBufferInfo buffer_info = {};
	buffer_info.buffer = m_uniformBuffer.buffer;
	buffer_info.offset = 0;
	buffer_info.range = sizeof(UniformTransformations);

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = m_uniformSet;
	descriptor_write.dstBinding = 0;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_write.descriptorCount = 1;
	descriptor_write.pBufferInfo = &buffer_info;

	vkUpdateDescriptorSets(m_logicalDevice, 1, &descriptor_write, 0, nullptr);
}

#pragma endregion

#pragma region Textures 

void VulkanGraphics::CreateTextureSampler() {
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
	sampler_info.anisotropyEnable = VK_FALSE;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;
	sampler_info.maxAnisotropy = 1.0f;

	if (vkCreateSampler(m_logicalDevice, &sampler_info, nullptr, &m_textureSampler) != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

void VulkanGraphics::CreateTexture(uchar* pixels) {
	int2 image_extents = int2(IMAGE_WIDTH, IMAGE_HEIGHT);
	int channels = 4;
	
	VkDeviceSize buffer_size = image_extents.x * image_extents.y * channels;
	BufferHandle staging = CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, image_extents.x * image_extents.y);

	void* data_location;
	vkMapMemory(m_logicalDevice, staging.memory, 0, buffer_size, 0, &data_location);
	std::memcpy(data_location, pixels, buffer_size);
	vkUnmapMemory(m_logicalDevice, staging.memory);

	m_textureHandle = CreateImage(image_extents, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	TransitionImageLayout(m_textureHandle.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(staging.buffer, m_textureHandle.image, image_extents);
	TransitionImageLayout(m_textureHandle.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_textureHandle.image_view = CreateImageView(m_textureHandle.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

	VkDescriptorSetAllocateInfo set_info = {};
	set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	set_info.descriptorPool = m_texturePool;
	set_info.descriptorSetCount = 1;
	set_info.pSetLayouts = &m_textureSetLayout;

	VkResult result = vkAllocateDescriptorSets(m_logicalDevice, &set_info, &m_textureHandle.set);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	VkDescriptorImageInfo image_info = {};
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = m_textureHandle.image_view;
	image_info.sampler = m_textureSampler;

	VkWriteDescriptorSet descriptor_write = {};
	descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_write.dstSet = m_textureHandle.set;
	descriptor_write.dstBinding = 0;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_write.descriptorCount = 1;
	descriptor_write.pImageInfo = &image_info;

	vkUpdateDescriptorSets(m_logicalDevice, 1, &descriptor_write, 0, nullptr);

	DestroyBuffer(staging);
}

void VulkanGraphics::DestroyTexture(TextureHandle handle) {
	vkDeviceWaitIdle(m_logicalDevice);
	vkFreeDescriptorSets(m_logicalDevice, m_texturePool, 1, &handle.set);
	vkDestroyImageView(m_logicalDevice, handle.image_view, nullptr);
	vkDestroyImage(m_logicalDevice, handle.image, nullptr);
	vkFreeMemory(m_logicalDevice, handle.memory, nullptr);
}

void VulkanGraphics::TransitionImageLayout(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout) {
	VkCommandBuffer local_command_buffer = BeginTransientCommandBuffer();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destination_stage;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT; 
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}

	if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	vkCmdPipelineBarrier(local_command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	EndTransientCommandBuffer(local_command_buffer);
}

void VulkanGraphics::CopyBufferToImage(VkBuffer buffer, VkImage image, int2 image_size) {
	VkCommandBuffer local_command_buffer = BeginTransientCommandBuffer();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { static_cast<uint>(image_size.x), static_cast<uint>(image_size.y), 1 };

	vkCmdCopyBufferToImage(local_command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndTransientCommandBuffer(local_command_buffer);
}

TextureHandle VulkanGraphics::CreateImage(int2 size, VkFormat image_format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	TextureHandle handle = {};

	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = image_format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;

	VkResult result = vkCreateImage(m_logicalDevice, &image_info, nullptr, &handle.image);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(m_logicalDevice, handle.image, &memory_requirements);

	uint chosen_memory_type = FindMemoryType(memory_requirements.memoryTypeBits, properties);

	VkMemoryAllocateInfo allocation_info = {};
	allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocation_info.allocationSize = memory_requirements.size;
	allocation_info.memoryTypeIndex = chosen_memory_type;

	VkResult allocation_result = vkAllocateMemory(m_logicalDevice, &allocation_info, nullptr, &handle.memory);
	if (allocation_result != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory!");
	}

	vkBindImageMemory(m_logicalDevice, handle.image, handle.memory, 0);

	return handle;
}

#pragma endregion

#pragma region Class
VulkanGraphics::VulkanGraphics(gsl::not_null<Window*> window, Shader* shader) : Graphics(window, shader) {
}

VulkanGraphics::~VulkanGraphics() {
	if (m_logicalDevice != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(m_logicalDevice);

		DestroyTexture(m_textureHandle);
		DestroyBuffer(m_vertexBuffer);
		DestroyBuffer(m_indexBuffer);
		 
		CleanupSwapChain();

		if (m_texturePool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_logicalDevice, m_texturePool, nullptr);
		}

		if (m_textureSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(m_logicalDevice, m_textureSetLayout, nullptr);
		}

		if (m_textureSampler != VK_NULL_HANDLE) {
			vkDestroySampler(m_logicalDevice, m_textureSampler, nullptr);
		}

		if (m_uniformSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(m_logicalDevice, m_uniformSetLayout, nullptr);
		}

		if (m_uniformPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_logicalDevice, m_uniformPool, nullptr);
		}

		DestroyBuffer(m_uniformBuffer);

		if (m_imageAvailableSignal != VK_NULL_HANDLE) {
			vkDestroySemaphore(m_logicalDevice, m_imageAvailableSignal, nullptr);
		}

		for (VkSemaphore semaphore : m_renderFinishedSignals) {
			if (semaphore != VK_NULL_HANDLE) {
				vkDestroySemaphore(m_logicalDevice, semaphore, nullptr);
			}
		}

		if (m_imageAvailableSignal != VK_NULL_HANDLE) {
			vkDestroyFence(m_logicalDevice, m_stillRenderingFence, nullptr);
		}

		if (m_commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
		}

		if (m_pipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(m_logicalDevice, m_pipeline, nullptr);
		}

		if (m_pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
		}

		if (m_renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
		}

		vkDestroyDevice(m_logicalDevice, nullptr);
	}

	if (m_instance != VK_NULL_HANDLE) {
		if (m_surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		}

		if (m_debugMessenger != VK_NULL_HANDLE) {
			vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		}
		
		vkDestroyInstance(m_instance, nullptr);
	}
}

void VulkanGraphics::Initialize() {
#if !defined(NDEBUG) 
	m_validationEnabled = true;
#endif

	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDeviceAndQueues();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayouts();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSignals();
	CreateUniformBuffers();
	CreateDescriptorPools();
	CreateDescriptorSets();
	CreateTextureSampler();
}

#pragma endregion