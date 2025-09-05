#pragma once

#include "../graphics.hpp"
#include "../glfw/glfw_window.hpp"

class OpenGLGraphics final : public Graphics {
public:
	explicit OpenGLGraphics(gsl::not_null<Window*> window, Shader* shader);
	~OpenGLGraphics();

	bool BeginFrame() override;
	void EndFrame() override;

	void CreateVertexBuffer(span<Vertex> vertices) override;
	void CreateIndexBuffer(span<int> indices) override;
	void CreateTexture(uchar* pixels) override;
	void RenderIndexedBuffer() override;
	void SetViewProjection(mat4 view, mat4 projection) override;

	// TODO: Add Destroy-methods
private:
	void Initialize() override;
	void BindVertexArrayObject();
	void AttachShader();

	unsigned int VAO_ = 0;
	unsigned int VBO_ = 0;
	unsigned int EBO_ = 0;
	unsigned int texture_ = 0;
	uchar* pixels_;
};