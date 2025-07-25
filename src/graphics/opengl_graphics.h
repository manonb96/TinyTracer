#pragma once

#include "graphics.h"
#include "glfw/glfw_window.h"

class OpenGLGraphics final : public Graphics {
public:
	explicit OpenGLGraphics(gsl::not_null<Window*> window);
	~OpenGLGraphics();

	void CreateVertexBuffer(gsl::span<float> vertices) override;
	void CreateIndexBuffer(gsl::span<int> indices) override;
	void CreateTexture() override;
	void RenderIndexedBuffer(unsigned char* pixels, unsigned int shaderID) override;

	// TODO: Add Destroy-methods
private:
	void Initialize() override;
	void BindVertexArrayObject();

	unsigned int VAO_ = 0;
	unsigned int VBO_ = 0;
	unsigned int EBO_ = 0;
	unsigned int texture_ = 0;
};