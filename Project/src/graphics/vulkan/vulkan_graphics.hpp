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

	// Core objects
	VkInstance m_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_logicalDevice = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;

	// Swapchain related
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkPresentModeKHR m_presentMode;
	VkExtent2D m_extent;
	vector<VkImage> m_swapChainImages;
	vector<VkImageView> m_swapChainImageViews;
	vector<VkFramebuffer> m_swapChainFrameBuffers;

	// Pipeline and render pass
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipeline m_pipeline = VK_NULL_HANDLE;

	// Command buffers and pools
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;

	// Synchronization
	VkSemaphore m_imageAvailableSignal = VK_NULL_HANDLE;
	vector<VkSemaphore> m_renderFinishedSignals;
	VkFence m_stillRenderingFence = VK_NULL_HANDLE;

	// Buffers
	BufferHandle m_vertexBuffer = {};
	BufferHandle m_indexBuffer = {};
	BufferHandle m_uniformBuffer = {};

	// Textures
	TextureHandle m_textureHandle = {};
	VkDescriptorSetLayout m_textureSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_texturePool = VK_NULL_HANDLE;
	VkSampler m_textureSampler = VK_NULL_HANDLE;

	// Uniforms and descriptors
	VkDescriptorSetLayout m_uniformSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_uniformPool = VK_NULL_HANDLE;
	VkDescriptorSet m_uniformSet = VK_NULL_HANDLE;
	void* m_pUniformBufferLocation = nullptr;

	// Other
	std::array<cstring, 1> m_requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	bool m_validationEnabled = false;
	uint m_currentImageIndex = 0;

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

	// Destruction of Vulkan
	void DestroyBuffer(BufferHandle handle);
	void DestroyTexture(TextureHandle handle);

	// Rendering commands
	void BeginCommands();
	void EndCommands();

	// Static utils
	static span<cstring> GetSuggestedInstanceExtensions();
	static vector<VkExtensionProperties> GetSupportedInstanceExtensions();
	static bool AreAllExtensionsSupported(span<cstring> extensions);
	static vector<VkLayerProperties> GetSupportedValidationLayers();
	static bool AreAllLayersSupported(span<cstring> extensions);

	// Device queries
	vector<cstring> GetRequiredInstanceExtensions();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	vector<VkPhysicalDevice> GetAvailableDevices();
	bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);
	vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice device);

	// Swapchain helpers
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(span<VkSurfaceFormatKHR> formats);
	VkPresentModeKHR ChooseSwapPresentMode(span<VkPresentModeKHR> modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	uint ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

	// Shader and memory helpers
	VkShaderModule CreateShaderModule(span<uchar> buffer);
	uint FindMemoryType(uint type_bits_filter, VkMemoryPropertyFlags required_properties);

	// Buffer and image helpers
	BufferHandle CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, uint element_count);
	VkCommandBuffer BeginTransientCommandBuffer();
	void EndTransientCommandBuffer(VkCommandBuffer command_buffer);
	TextureHandle CreateImage(int2 size, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void TransitionImageLayout(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, int2 image_size);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flag);
	
	// Viewport helpers
	VkViewport GetViewport();
	VkRect2D GetScissor();
};