#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "glfw_initialization.hpp"
#include "../../utils/precomp.hpp"

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