#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl_graphics.h"
#include "../../utils/constants.hpp"
#include "../../utils/precomp.h"

#pragma region Helpers

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width * 2, height * 2);
}

#pragma endregion

#pragma region Drawing
bool OpenGLGraphics::BeginFrame() {
	return true;
}

void OpenGLGraphics::EndFrame() {
	glfwSwapBuffers(window_->GetHandle());
}

#pragma endregion

#pragma region Buffers

void OpenGLGraphics::BindVertexArrayObject() {
	glGenVertexArrays(1, &VAO_);
	glBindVertexArray(VAO_);
}

void OpenGLGraphics::CreateVertexBuffer(gsl::span<Vertex> vertices) {
	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float3)));
	glEnableVertexAttribArray(1);
}

void OpenGLGraphics::CreateIndexBuffer(gsl::span<int> indices) {
	glGenBuffers(1, &EBO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

void OpenGLGraphics::RenderIndexedBuffer(unsigned int shaderID) {
	glClear(GL_COLOR_BUFFER_BIT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_);
	glUseProgram(shaderID);
	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

#pragma endregion

#pragma region Texture

void OpenGLGraphics::CreateTexture(unsigned char* pixels)
{
	pixels_ = pixels;
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

#pragma endregion

#pragma region Class

OpenGLGraphics::OpenGLGraphics(gsl::not_null<Window*> window) : Graphics(window) {
}

OpenGLGraphics::~OpenGLGraphics() {

}

void OpenGLGraphics::Initialize() {
	// Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::exit(EXIT_FAILURE); // TODO: Verify if this calls SPDLog
	}

	glClearColor(0.f, 0.f, 0.f, 1.0f);

	int width, height;
	glfwGetFramebufferSize(window_->GetHandle(), &width, &height);
	glViewport(0, 0, width * 2, height * 2); // TODO: Handle viewport size in more robust manner
	glfwSetFramebufferSizeCallback(window_->GetHandle(), framebuffer_size_callback);

	// Bind vertex array object
	BindVertexArrayObject();
}

#pragma endregion