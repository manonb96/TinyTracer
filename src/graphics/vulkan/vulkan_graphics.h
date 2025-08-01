#pragma once
#include <vulkan/vulkan.h>
#include "../graphics.h"
#include "../glfw/glfw_window.h"

class VulkanGraphics final : public Graphics {
public:
	explicit VulkanGraphics(gsl::not_null<Window*> window);
	~VulkanGraphics();

	void CreateVertexBuffer(gsl::span<Vertex> vertices) override;
	void CreateIndexBuffer(gsl::span<int> indices) override;
	void CreateTexture() override;
	void RenderIndexedBuffer(unsigned char* pixels, unsigned int shaderID) override;

private:
	void Initialize() override;

	// Initialization of Vulkan
	void CreateInstance();
	void SetupDebugMessenger();

	// Static utils
	static gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();
	static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
	static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions);
	
	static std::vector<VkLayerProperties> GetSupportedValidationLayers();
	static bool AreAllLayersSupported(gsl::span<gsl::czstring> extensions);

	// Private methods
	std::vector<gsl::czstring> GetRequiredInstanceExtensions();

	// Member variables
	VkInstance instance_ = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

	// Validation
	bool validation_enabled_ = false;
};