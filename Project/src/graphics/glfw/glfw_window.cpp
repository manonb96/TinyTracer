#include <GLFW/glfw3.h>
#include "glfw_window.hpp"
#include "glfw_monitor.hpp"

Window::Window(cstring name, int2 size) {

#if OPENGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
#elif VULKAN
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

	m_pWindow = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if (m_pWindow == nullptr) {
		std::exit(EXIT_FAILURE);
	}
	
#if OPENGL
	glfwMakeContextCurrent(m_pWindow);
#endif
}

Window::~Window() {
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

int2 Window::GetWindowSize() const {
	int2 window_size;
	glfwGetWindowSize(m_pWindow, &window_size.x, &window_size.y);
	return window_size;
}

int2 Window::GetFrameBufferSize() const {
	int2 framebuffer_size;
	glfwGetFramebufferSize(m_pWindow, &framebuffer_size.x, &framebuffer_size.y);
	return framebuffer_size;
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(m_pWindow);
}

GLFWwindow* Window::GetHandle() const {
	return m_pWindow;
}

bool Window::TryMoveToPrimaryMonitor() {
	GLFWmonitor* primary_monitor = GetPrimaryMonitor();
	MoveWindowToMonitor(GetHandle(), primary_monitor);
	return true;
}

void Window::ProcessUserInput() {
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_pWindow, true);
}