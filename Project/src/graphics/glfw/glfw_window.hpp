#pragma once

struct GLFWwindow;

class Window {
public:
	Window(cstring name, int2 size);
	~Window();
	int2 GetWindowSize() const;
	int2 GetFrameBufferSize() const;
	bool ShouldClose() const;
	GLFWwindow* GetHandle() const;

	bool TryMoveToPrimaryMonitor();
	void ProcessUserInput();

private:
	GLFWwindow* m_pWindow;
};