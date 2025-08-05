#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <set>
#include "../../utils/precomp.h"
#include "vulkan_graphics.h"

#pragma region Vulkan functions extension implementations

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger) {
	// what is reinterpret
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
	if (!validation_enabled_) {
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT info = GetCreateMessengerInfo();
	VkResult result = vkCreateDebugUtilsMessengerEXT(instance_, &info, nullptr, &debug_messenger_);
	if (result != VK_SUCCESS) {
		spdlog::error("Cannot create debug messenger");
		return;
	}
}

bool LayerMatchesName(gsl::czstring name, const VkLayerProperties& properties) {
	return streq(properties.layerName, name);
}

bool IsLayerSupported(std::vector<VkLayerProperties> layers, gsl::czstring name) {
	return std::any_of(layers.begin(), layers.end(), std::bind_front(LayerMatchesName, name));
}

bool VulkanGraphics::AreAllLayersSupported(gsl::span<gsl::czstring> layers) {
	std::vector<VkLayerProperties> supported_layers = GetSupportedValidationLayers();
	return std::all_of(layers.begin(), layers.end(), std::bind_front(IsLayerSupported, supported_layers));
}

std::vector<VkLayerProperties> VulkanGraphics::GetSupportedValidationLayers() {
	std::uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);

	if (count == 0) {
		return {};
	}

	std::vector<VkLayerProperties> properties(count);
	vkEnumerateInstanceLayerProperties(&count, properties.data());
	return properties;
}

#pragma endregion

#pragma region Instance and extensions

void VulkanGraphics::CreateInstance() {
	std::array<gsl::czstring, 1> validation_layers = { "VK_LAYER_KHRONOS_validation" };
	if (!AreAllLayersSupported(validation_layers)) {
		validation_enabled_ = false;
	}

	std::vector<gsl::czstring> required_extensions = GetRequiredInstanceExtensions();

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

	if (validation_enabled_) {
		instance_creation_info.pNext = &messenger_creation_info;
		instance_creation_info.enabledLayerCount = validation_layers.size();
		instance_creation_info.ppEnabledLayerNames = validation_layers.data();
	}
	else {
		instance_creation_info.enabledLayerCount = 0;
		instance_creation_info.ppEnabledLayerNames = nullptr;
	}

	VkResult result = vkCreateInstance(&instance_creation_info, nullptr, &instance_);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}
}

gsl::span<gsl::czstring> VulkanGraphics::GetSuggestedInstanceExtensions() {
	std::uint32_t glfw_extension_count = 0;
	gsl::czstring* glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	return { glfw_extensions, glfw_extension_count };
}

std::vector<gsl::czstring> VulkanGraphics::GetRequiredInstanceExtensions() {
	gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtensions();
	std::vector<gsl::czstring> required_extensions(suggested_extensions.size());
	std::copy(suggested_extensions.begin(), suggested_extensions.end(), required_extensions.begin());

	if (validation_enabled_) {
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!AreAllExtensionsSupported(required_extensions)) {
		std::exit(EXIT_FAILURE);
	}

	return required_extensions;
}

std::vector<VkExtensionProperties> VulkanGraphics::GetSupportedInstanceExtensions() {
	std::uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	if (count == 0) {
		return {};
	}

	std::vector<VkExtensionProperties> properties(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
	return properties;
}

bool ExtensionMatchesName(gsl::czstring name, const VkExtensionProperties& properties) {
	return streq(properties.extensionName, name);
}

bool IsExtensionSupported(std::vector<VkExtensionProperties> supportedextensions, gsl::czstring name) {
	return std::any_of(supportedextensions.begin(), supportedextensions.end(), std::bind_front(ExtensionMatchesName, name));
}

bool VulkanGraphics::AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions) {
	std::vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();
	return std::all_of(extensions.begin(), extensions.end(), std::bind_front(IsExtensionSupported, supported_extensions));
}

#pragma endregion

#pragma region Devices and queues
VulkanGraphics::QueueFamilyIndices VulkanGraphics::FindQueueFamilies(VkPhysicalDevice device) {
	std::uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

	auto graphics_family_it = std::find_if(families.begin(), families.end(), [](const VkQueueFamilyProperties& props) {
		return props.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);
		});

	QueueFamilyIndices result;
	result.graphics_family = graphics_family_it - families.begin();

	for (std::uint32_t i = 0; i < families.size(); i++) {
		VkBool32 has_presentation_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &has_presentation_support);
		if (has_presentation_support) {
			result.presentation_family = i;  // It might be the same as Graphics Family, but consider separately
			break;
		}
	}

	return result;
}

VulkanGraphics::SwapChainProperties VulkanGraphics::GetSwapChainProperties(VkPhysicalDevice device) {
	SwapChainProperties properties;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &properties.capabilities);

	std::uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
	properties.formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, properties.formats.data());

	std::uint32_t modes_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &modes_count, nullptr);
	properties.present_modes.resize(modes_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &modes_count, properties.present_modes.data());

	return properties;
}

std::vector<VkExtensionProperties> VulkanGraphics::GetDeviceAvailableExtensions(VkPhysicalDevice device) {
	std::uint32_t available_extensions_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extensions_count, nullptr);
	std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &available_extensions_count, available_extensions.data());

	return available_extensions;
}

bool VulkanGraphics::AreAllDeviceExtensionsSupported(VkPhysicalDevice device) {
	std::vector<VkExtensionProperties> available_extensions = GetDeviceAvailableExtensions(device);
	return std::all_of(required_device_extensions_.begin(), required_device_extensions_.end(), std::bind_front(IsExtensionSupported, available_extensions));
}

bool VulkanGraphics::IsDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices families = FindQueueFamilies(device);
	return families.IsValid() && AreAllDeviceExtensionsSupported(device) &&
		GetSwapChainProperties(device).IsValid(); 
}

void VulkanGraphics::PickPhysicalDevice() {
	std::vector<VkPhysicalDevice> devices = GetAvailableDevices();
	std::erase_if(devices, std::not_fn(std::bind_front(&VulkanGraphics::IsDeviceSuitable, this)));

	if (devices.empty()) {
		spdlog::error("No physical devices that match the criteria");
		std::exit(EXIT_FAILURE);
	}

	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(devices[0], &device_properties);
	spdlog::info("Setting {} as physical device (total # physical devices available: {})", device_properties.deviceName, devices.size());
	
	// We just pick the first available device
	physical_device_ = devices[0];
}

std::vector<VkPhysicalDevice> VulkanGraphics::GetAvailableDevices() {
	std::uint32_t device_count;
	vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

	if (device_count == 0) {
		return {};
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

	return devices;
}

void VulkanGraphics::CreateLogicalDeviceAndQueues() {
	QueueFamilyIndices picked_device_families = FindQueueFamilies(physical_device_);

	// Sanity check
	if (!picked_device_families.IsValid()) {
		std::exit(EXIT_FAILURE);
	}

	// If graphics family and presentation family at the same, the set will only have one value
	std::set<std::uint32_t> unique_queue_families = { picked_device_families.graphics_family.value(), picked_device_families.presentation_family.value() };

	std::float_t queue_priority = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	for (std::uint32_t unique_queue_family : unique_queue_families) {
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
	device_info.enabledExtensionCount = required_device_extensions_.size();
	device_info.ppEnabledExtensionNames = required_device_extensions_.data();
	device_info.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(physical_device_, &device_info, nullptr, &logical_device_);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	vkGetDeviceQueue(logical_device_, picked_device_families.graphics_family.value(), 0, &graphics_queue_);
	vkGetDeviceQueue(logical_device_, picked_device_families.presentation_family.value(), 0, &present_queue_);
}

#pragma endregion

#pragma region Presentation

void VulkanGraphics::CreateSurface() {
	VkResult result = glfwCreateWindowSurface(instance_, window_->GetHandle(), nullptr, &surface_);
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

VkSurfaceFormatKHR VulkanGraphics::ChooseSwapSurfaceFormat(gsl::span<VkSurfaceFormatKHR> formats) {
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

VkPresentModeKHR VulkanGraphics::ChooseSwapPresentMode(gsl::span<VkPresentModeKHR> modes) {
	if (std::any_of(modes.begin(), modes.end(), IsMailboxPresentMode)) {
		return VK_PRESENT_MODE_MAILBOX_KHR;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanGraphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	constexpr std::uint32_t kInvalidSize = std::numeric_limits<std::uint32_t>::max();
	if (capabilities.currentExtent.width != kInvalidSize) {
		return capabilities.currentExtent;
	}
	else {
		int2 size = window_->GetFrameBufferSize();
		VkExtent2D actual_extent = {
			static_cast<std::uint32_t>(size.x),
			static_cast<std::uint32_t>(size.y),
		};
		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual_extent;
	}
}

std::uint32_t VulkanGraphics::ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
	std::uint32_t image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < image_count) {
		image_count = capabilities.maxImageCount;
	}
	return image_count;
}

void VulkanGraphics::CreateSwapChain() {
	SwapChainProperties properties = GetSwapChainProperties(physical_device_);

	surface_format_ = ChooseSwapSurfaceFormat(properties.formats);
	present_mode_ = ChooseSwapPresentMode(properties.present_modes);
	extent_ = ChooseSwapExtent(properties.capabilities);
	std::uint32_t image_count = ChooseSwapImageCount(properties.capabilities);

	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; 
	info.surface = surface_;
	info.minImageCount = image_count;
	info.imageFormat = surface_format_.format;
	info.imageColorSpace = surface_format_.colorSpace;
	info.imageExtent = extent_;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.presentMode = present_mode_;
	info.preTransform = properties.capabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.clipped = VK_TRUE;
	info.oldSwapchain = VK_NULL_HANDLE;

	QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

	if (indices.graphics_family != indices.presentation_family) {
		std::array<std::uint32_t, 2> family_indices = { indices.graphics_family.value(), indices.presentation_family.value() };
		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = family_indices.size();
		info.pQueueFamilyIndices = family_indices.data();
	}
	else {
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VkResult result = vkCreateSwapchainKHR(logical_device_, &info, nullptr, &swap_chain_);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	std::uint32_t actual_image_count;
	vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &actual_image_count, nullptr);
	swap_chain_images_.resize(actual_image_count);
	vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &actual_image_count, swap_chain_images_.data());
}

void VulkanGraphics::CleanupSwapChain() {
	if (logical_device_ == VK_NULL_HANDLE) {
		return;
	}

	for (VkFramebuffer framebuffer : swap_chain_framebuffers_) {
		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(logical_device_, framebuffer, nullptr);
		}
	}

	for (VkImageView image_view : swap_chain_image_views_) {
		if (image_view != VK_NULL_HANDLE) {
			vkDestroyImageView(logical_device_, image_view, nullptr);
		}
	}

	if (swap_chain_ != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);
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
	VkResult result = vkCreateImageView(logical_device_, &info, nullptr, &view);
	if (result != VK_SUCCESS) {
		std::exit(EXIT_FAILURE);
	}

	return view;
}

void VulkanGraphics::CreateImageViews() {
	swap_chain_image_views_.resize(swap_chain_images_.size());
	auto image_view_it = swap_chain_image_views_.begin();
	for (VkImage image : swap_chain_images_) {
		*image_view_it = CreateImageView(image, surface_format_.format, VK_IMAGE_ASPECT_COLOR_BIT);
		image_view_it = std::next(image_view_it);
	}
}

#pragma endregion

#pragma region Drawing
bool VulkanGraphics::BeginFrame() {
	return true;
}

void VulkanGraphics::EndFrame() {
}

#pragma region Buffers

void VulkanGraphics::CreateVertexBuffer(gsl::span<Vertex> vertices) {

}

void VulkanGraphics::CreateIndexBuffer(gsl::span<int> indices) {

}

void VulkanGraphics::RenderIndexedBuffer(unsigned char* pixels, unsigned int shaderID) {

}
#pragma endregion

#pragma region Textures 
void VulkanGraphics::CreateTexture() {

}

#pragma endregion

#pragma region Class
VulkanGraphics::VulkanGraphics(gsl::not_null<Window*> window) : Graphics(window) {
}

VulkanGraphics::~VulkanGraphics() {
	if (logical_device_ != VK_NULL_HANDLE) {
		CleanupSwapChain();

		vkDestroyDevice(logical_device_, nullptr);
	}

	if (instance_ != VK_NULL_HANDLE) {
		if (surface_ != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(instance_, surface_, nullptr);
		}

		if (debug_messenger_ != VK_NULL_HANDLE) {
			vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
		}
		
		vkDestroyInstance(instance_, nullptr);
	}
}

void VulkanGraphics::Initialize() {
#if !defined(NDEBUG) 
	validation_enabled_ = true;
#endif

	CreateInstance();
	SetupDebugMessenger();
	CreateSurface(); // Surface needs to be created before getting Presentation Queue Family
	PickPhysicalDevice();
	CreateLogicalDeviceAndQueues();
	CreateSwapChain();
	CreateImageViews();
}

#pragma endregion