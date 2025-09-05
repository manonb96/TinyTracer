#include <GLFW/glfw3.h>
#include "../../utils/precomp.hpp"
#include "glfw_monitor.hpp"

GLFWmonitor* GetPrimaryMonitor() {
	return glfwGetPrimaryMonitor();
}

int2 GetMonitorPosition(gsl::not_null<GLFWmonitor*> monitor) {
	int2 monitor_logical_position;
	glfwGetMonitorPos(monitor, &monitor_logical_position.x, &monitor_logical_position.y);
	return monitor_logical_position;
}

int2 GetMonitorWorkAreaSize(gsl::not_null<GLFWmonitor*> monitor) {
	int2 monitor_size;
	glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitor_size.x, &monitor_size.y);
	return monitor_size;
}

void MoveWindowToMonitor(gsl::not_null<GLFWwindow*> window, gsl::not_null<GLFWmonitor*> monitor) {
	int2 window_size;
	glfwGetWindowSize(window, &window_size.x, &window_size.y);
	const int2 window_new_position = GetMonitorPosition(monitor) + (GetMonitorWorkAreaSize(monitor) / 2) - (window_size / 2);
	glfwSetWindowPos(window, window_new_position.x, window_new_position.y);
}