#pragma once
#include <vulkan/vulkan.h>
#include "../graphics.h"
#include "../glfw/glfw_window.h"

class VulkanGraphics final : public Graphics {
public:
	explicit VulkanGraphics(gsl::not_null<Window*> window);
	~VulkanGraphics();

	bool BeginFrame() override;
	void EndFrame() override;

	void CreateVertexBuffer(gsl::span<Vertex> vertices) override;
	void CreateIndexBuffer(gsl::span<int> indices) override;
	void CreateTexture() override;
	void RenderIndexedBuffer(unsigned char* pixels, unsigned int shaderID) override;

private:
	struct QueueFamilyIndices {
		std::optional<std::uint32_t> graphics_family = std::nullopt;
		std::optional<std::uint32_t> presentation_family = std::nullopt;

		bool IsValid() const { return graphics_family.has_value() && presentation_family.has_value(); }
	};

	struct SwapChainProperties {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;

		bool IsValid() const { return !formats.empty() && !present_modes.empty(); }
	};

	// Initialization of Vulkan
	void Initialize() override;
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDeviceAndQueues();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateDescriptorSetLayouts();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffer();
	void CreateSignals();

	void RecreateSwapChain();
	void CleanupSwapChain();

	// Rendering
	void BeginCommands();
	void EndCommands();

	// Static utils
	static gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();
	static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
	static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions);
	
	static std::vector<VkLayerProperties> GetSupportedValidationLayers();
	static bool AreAllLayersSupported(gsl::span<gsl::czstring> extensions);

	// Private methods
	std::vector<gsl::czstring> GetRequiredInstanceExtensions();

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);

	bool IsDeviceSuitable(VkPhysicalDevice device);
	std::vector<VkPhysicalDevice> GetAvailableDevices();
	bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);
	std::vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(gsl::span<VkSurfaceFormatKHR> formats);
	VkPresentModeKHR ChooseSwapPresentMode(gsl::span<VkPresentModeKHR> modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	std::uint32_t ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

	VkShaderModule CreateShaderModule(gsl::span<std::uint8_t> buffer);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flag);
	VkViewport GetViewport();
	VkRect2D GetScissor();

	// Member variables
	VkInstance instance_ = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

	std::array<gsl::czstring, 1> required_device_extensions_ = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
	VkDevice logical_device_ = VK_NULL_HANDLE;
	VkQueue graphics_queue_ = VK_NULL_HANDLE;
	VkQueue present_queue_ = VK_NULL_HANDLE;

	VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
	VkSurfaceFormatKHR surface_format_;
	VkPresentModeKHR present_mode_;
	VkExtent2D extent_;
	std::vector<VkImage> swap_chain_images_;
	std::vector<VkImageView> swap_chain_image_views_;
	std::vector<VkFramebuffer> swap_chain_framebuffers_;

	VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
	VkRenderPass render_pass_ = VK_NULL_HANDLE;
	VkPipeline pipeline_ = VK_NULL_HANDLE;

	VkCommandPool command_pool_ = VK_NULL_HANDLE;
	VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;

	VkSemaphore image_available_signal_ = VK_NULL_HANDLE;
	VkSemaphore render_finished_signal_ = VK_NULL_HANDLE;
	VkFence still_rendering_fence_ = VK_NULL_HANDLE;

	std::uint32_t current_image_index_ = 0;

	VkDescriptorSetLayout uniform_set_layout_ = VK_NULL_HANDLE;

	VkDescriptorSetLayout texture_set_layout_ = VK_NULL_HANDLE;

	// Validation
	bool validation_enabled_ = false;
};