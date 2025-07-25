#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/glfw/glfw_initialization.h"
#include "graphics/glfw/glfw_window.h"
#include "graphics/opengl_graphics.h"

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
	std::array<float, 20> vertices = {
		// positions	// texture coordinates
		-1, -1, 0,		-1, -1,
		1, -1, 0,		1, -1,
		-1, 1, 0,		-1, 1,
		1, 1, 0,		1, 1
	};

	// Set element buffer
	std::array<int, 6> indices = {
		0, 1, 2,
		1, 2, 3
	};

	// Initialize Graphics API
#ifdef OPENGL
	OpenGLGraphics graphics(&window);
#endif

	graphics.Init();
	graphics.CreateVertexBuffer(vertices);
	graphics.CreateIndexBuffer(indices);
	graphics.CreateTexture();

	// Set up shaders and textures
	Shader shader("shaders/basic.vert", "shaders/basic.frag");
	unsigned char* pixels = new unsigned char[WIDTH * HEIGHT * 3];

	// Initialize ray tracing core
	Core* core = new Core();
	core->InitializeScene();

	while (!window.ShouldClose()) {

		// Handle user input
		window.ProcessUserInput();

		// Render scene
		core->RenderScene(pixels);
		graphics.RenderIndexedBuffer(pixels, shader.ID);

		// Wrap up
		glfwSwapBuffers(window.GetHandle());
		glfwPollEvents();
	}

	// Clear memory
	delete[] pixels;
	core->~Core();

	return 0;
}