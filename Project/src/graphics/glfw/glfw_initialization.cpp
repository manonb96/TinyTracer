#include <GLFW/glfw3.h>
#include "glfw_initialization.hpp"

void glfw_error_callback(int error_code, cstring message) {
	spdlog::error("Glfw validation: {}", message);
}

GlfwInitialization::GlfwInitialization() {
	glfwSetErrorCallback(glfw_error_callback);

	if (glfwInit() != GLFW_TRUE) {
		std::exit(EXIT_FAILURE);
	}
}

GlfwInitialization::~GlfwInitialization() {
	glfwTerminate();
}