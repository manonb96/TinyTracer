#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "glfw_initialization.h"
#include "../../utils/precomp.h"

void glfw_error_callback(int error_code, const char* message) {
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