#include <GLFW/glfw3.h>
#include "glfw_window.h"
#include "glfw_monitor.h"

Window::Window(const char* name, int2 size) {

#if VULKAN
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // OpenGL
#endif

	window_ = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if (window_ == nullptr) {
		std::exit(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(window_); // OpenGL
}

Window::~Window() {
	glfwDestroyWindow(window_);
	glfwTerminate();
}

int2 Window::GetWindowSize() const {
	int2 window_size;
	glfwGetWindowSize(window_, &window_size.x, &window_size.y);
	return window_size;
}

int2 Window::GetFrameBufferSize() const {
	int2 framebuffer_size;
	glfwGetFramebufferSize(window_, &framebuffer_size.x, &framebuffer_size.y);
	return framebuffer_size;
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(window_);
}

GLFWwindow* Window::GetHandle() const {
	return window_;
}

bool Window::TryMoveToPrimaryMonitor() {
	GLFWmonitor* primary_monitor = GetPrimaryMonitor();
	MoveWindowToMonitor(GetHandle(), primary_monitor);
	return true;
}

void Window::ProcessUserInput() {
	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_, true);
}