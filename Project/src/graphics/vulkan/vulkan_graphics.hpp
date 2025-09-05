#pragma once
#include <vulkan/vulkan.h>
#include "../graphics.hpp"
#include "../glfw/glfw_window.hpp"
#include "buffer_handle.hpp"
#include "texture_handle.hpp"

class VulkanGraphics final : public Graphics {
public:
	explicit VulkanGraphics(gsl::not_null<Window*> window, Shader* shader);
	~VulkanGraphics();

	bool BeginFrame() override;
	void EndFrame() override;

	void CreateVertexBuffer(span<Vertex> vertices) override;
	void CreateIndexBuffer(span<int> indices) override;
	void CreateTexture(uchar* pixels) override;
	void RenderIndexedBuffer() override;
	void SetViewProjection(mat4 view, mat4 projection) override;

private:
	struct QueueFamilyIndices {
		std::optional<uint> graphics_family = std::nullopt;
		std::optional<uint> presentation_family = std::nullopt;

		bool IsValid() const { return graphics_family.has_value() && presentation_family.has_value(); }
	};

	struct SwapChainProperties {
		VkSurfaceCapabilitiesKHR capabilities;
		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> present_modes;

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
	void CreateUniformBuffers();
	void CreateDescriptorPools();
	void CreateDescriptorSets();
	void CreateTextureSampler();

	void RecreateSwapChain();
	void CleanupSwapChain();
	void DestroyBuffer(BufferHandle handle);
	void DestroyTexture(TextureHandle handle);

	// Rendering
	void BeginCommands();
	void EndCommands();

	// Static utils
	static span<cstring> GetSuggestedInstanceExtensions();
	static vector<VkExtensionProperties> GetSupportedInstanceExtensions();
	static bool AreAllExtensionsSupported(span<cstring> extensions);
	
	static vector<VkLayerProperties> GetSupportedValidationLayers();
	static bool AreAllLayersSupported(span<cstring> extensions);

	// Private methods
	vector<cstring> GetRequiredInstanceExtensions();

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);

	bool IsDeviceSuitable(VkPhysicalDevice device);
	vector<VkPhysicalDevice> GetAvailableDevices();
	bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);
	vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(span<VkSurfaceFormatKHR> formats);
	VkPresentModeKHR ChooseSwapPresentMode(span<VkPresentModeKHR> modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	uint ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

	VkShaderModule CreateShaderModule(span<uchar> buffer);
	uint FindMemoryType(uint type_bits_filter, VkMemoryPropertyFlags required_properties);

	BufferHandle CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, uint element_count);
	VkCommandBuffer BeginTransientCommandBuffer();
	void EndTransientCommandBuffer(VkCommandBuffer command_buffer);

	TextureHandle CreateImage(int2 size, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void TransitionImageLayout(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, int2 image_size);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flag);
	
	VkViewport GetViewport();
	VkRect2D GetScissor();

	// Member variables
	VkInstance instance_ = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
	BufferHandle vertex_buffer_ = {};
	BufferHandle index_buffer_ = {};
	TextureHandle texture_handle_ = {};

	std::array<cstring, 1> required_device_extensions_ = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
	VkDevice logical_device_ = VK_NULL_HANDLE;
	VkQueue graphics_queue_ = VK_NULL_HANDLE;
	VkQueue present_queue_ = VK_NULL_HANDLE;

	VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
	VkSurfaceFormatKHR surface_format_;
	VkPresentModeKHR present_mode_;
	VkExtent2D extent_;
	vector<VkImage> swap_chain_images_;
	vector<VkImageView> swap_chain_image_views_;
	vector<VkFramebuffer> swap_chain_framebuffers_;

	VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
	VkRenderPass render_pass_ = VK_NULL_HANDLE;
	VkPipeline pipeline_ = VK_NULL_HANDLE;

	VkCommandPool command_pool_ = VK_NULL_HANDLE;
	VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;

	VkSemaphore image_available_signal_ = VK_NULL_HANDLE;
	vector<VkSemaphore> render_finished_signal_;
	VkFence still_rendering_fence_ = VK_NULL_HANDLE;

	uint current_image_index_ = 0;

	VkDescriptorSetLayout uniform_set_layout_ = VK_NULL_HANDLE;
	VkDescriptorPool uniform_pool_ = VK_NULL_HANDLE;
	VkDescriptorSet uniform_set_ = VK_NULL_HANDLE;
	BufferHandle uniform_buffer_ = {};
	void* uniform_buffer_location_ = nullptr;

	VkDescriptorSetLayout texture_set_layout_ = VK_NULL_HANDLE;
	VkDescriptorPool texture_pool_ = VK_NULL_HANDLE;
	VkSampler texture_sampler_ = VK_NULL_HANDLE;

	// Validation
	bool validation_enabled_ = false;
};