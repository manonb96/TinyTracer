#include "utils/constants.hpp"
#include "raytracing/core.hpp"
#include "shaders/shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width * 2, height * 2);
}

void processUserInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main() {

	// Initialize OpenGL
	if (!glfwInit()) {
		std::cerr << "[ERROR] GLFW initalization failed" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window", nullptr, nullptr);
	if (!window) {
		std::cerr << "[ERROR] Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "[ERROR] Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glClearColor(0.f, 0.f, 0.f, 1.0f);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	// TODO: Handle viewport size in more robust manner
	glViewport(0, 0, width * 2, height * 2);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Bind vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Set vertex buffer
	float vertices[] = {
		// positions	// texture coords
		-1, -1, 0,		-1, -1,
		1, -1, 0,		1, -1,
		-1, 1, 0,		-1, 1,
		1, 1, 0,		1, 1
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set element buffer
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set up shaders and textures
	Shader shader("shaders/basic.vert", "shaders/basic.frag");
	unsigned char* pixels = new unsigned char[WIDTH * HEIGHT * 3];

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Initialize rendercore
	Core* core = new Core();
	core->InitializeScene();

	while (!glfwWindowShouldClose(window)) {

		// Handle user input
		processUserInput(window);

		// Render scene
		glClear(GL_COLOR_BUFFER_BIT);
		core->RenderScene(pixels);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		shader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Wrap up
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clear memory
	glfwDestroyWindow(window);
	glfwTerminate();
	delete[] pixels;
	core->~Core();

	return 0;
}