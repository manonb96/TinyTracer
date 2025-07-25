#pragma once
#include "../../utils/precomp.h"

struct GLFWwindow;

class Window {
public:
	Window(const char* name, int2 size);
	~Window();
	int2 GetWindowSize() const;
	int2 GetFrameBufferSize() const;
	bool ShouldClose() const;
	GLFWwindow* GetHandle() const;

	bool TryMoveToPrimaryMonitor();
	void ProcessUserInput();

private:
	GLFWwindow* window_;
};