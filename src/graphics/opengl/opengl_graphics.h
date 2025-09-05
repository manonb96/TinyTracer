#pragma once

#include "../graphics.h"
#include "../glfw/glfw_window.h"

class OpenGLGraphics final : public Graphics {
public:
	explicit OpenGLGraphics(gsl::not_null<Window*> window, Shader* shader);
	~OpenGLGraphics();

	bool BeginFrame() override;
	void EndFrame() override;

	void CreateVertexBuffer(gsl::span<Vertex> vertices) override;
	void CreateIndexBuffer(gsl::span<int> indices) override;
	void CreateTexture(unsigned char* pixels) override;
	void RenderIndexedBuffer() override;
	void SetViewProjection(glm::mat4 view, glm::mat4 projection) override;

	// TODO: Add Destroy-methods
private:
	void Initialize() override;
	void BindVertexArrayObject();
	void AttachShader();

	unsigned int VAO_ = 0;
	unsigned int VBO_ = 0;
	unsigned int EBO_ = 0;
	unsigned int texture_ = 0;
	unsigned char* pixels_;
};