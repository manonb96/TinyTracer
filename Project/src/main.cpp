#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/glfw/glfw_initialization.hpp"
#include "graphics/glfw/glfw_window.hpp"
#include "graphics/opengl/opengl_graphics.hpp"
#include "graphics/vulkan/vulkan_graphics.hpp"

#include "utils/constants.hpp"
#include "raytracing/core.hpp"
#include "shaders/shader.hpp"

int main() {

	// Initialize glfw
	const GlfwInitialization glfw;

	// Create window
	Window window("GLFW Window", int2(WIDTH, HEIGHT));
	window.TryMoveToPrimaryMonitor();

	// Initialize buffers
	std::array<Vertex, 4> vertices = {
		Vertex({-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}),   // top - left
		Vertex({1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}),    // top - right
		Vertex({-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}),  // bottom - left
		Vertex({1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}),   // bottom - right
	};

	// Vertices are set up in clockwise order in the IndexBuffer
	std::array<int, 6> indices = {
		0, 1, 2,
		1, 3, 2
	};

	// Initialize shaders
	Shader shader("basic", "basic.vert", "basic.frag");

	// Initialize Graphics API
#if OPENGL
	OpenGLGraphics graphics(&window, &shader);
#elif VULKAN
	VulkanGraphics graphics(&window, &shader);
#endif

	graphics.Init();
	graphics.CreateVertexBuffer(vertices);
	graphics.CreateIndexBuffer(indices);

	// Set view projection
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);
	graphics.SetViewProjection(view, projection);

	// Initialize ray tracing core
	Core* core = new Core();
	core->InitializeScene();
	
	// Calculate pixel values and set up textures
	uchar* pixels = new uchar[WIDTH * HEIGHT * 4];
	core->RenderScene(pixels);
	graphics.CreateTexture(pixels);

	while (!window.ShouldClose()) {
		glfwPollEvents();

		// Handle user input
		window.ProcessUserInput();

		if (graphics.BeginFrame()) {
			// Render scene
			graphics.RenderIndexedBuffer();

			// Wrap up
			graphics.EndFrame();
		}
	}

	// Clear memory
	delete[] pixels;
	core->~Core();

	return 0;
}