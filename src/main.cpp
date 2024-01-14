#include "constants.hpp"
#include "core.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
	if (!glfwInit()) {
		std::cerr << "GLFW initalization failed" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	Core* core = new Core();
	core->InitializeGLEW();
	core->InitializeFramebuffer();
	core->InitializeScene();

	while (!glfwWindowShouldClose(window)) {
		core->RenderScene();
		glfwWaitEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}