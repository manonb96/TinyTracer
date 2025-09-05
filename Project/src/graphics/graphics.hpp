#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "glfw/glfw_window.hpp"
#include "vertex.hpp"
#include "uniform_transformations.hpp"
#include "../utils/precomp.hpp"
#include "../shaders/shader.hpp"

class Graphics {
public:
	Graphics(gsl::not_null<Window*> window, Shader* shader);
	virtual ~Graphics() = default;

	void Init();

	virtual bool BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void CreateVertexBuffer(span<Vertex> vertices) = 0;
	virtual void CreateIndexBuffer(span<int> indices) = 0;
	virtual void SetViewProjection(mat4 view, mat4 projection) = 0;
	virtual void CreateTexture(uchar* pixels) = 0;
	virtual void RenderIndexedBuffer() = 0;

protected:
	virtual void Initialize() = 0;

	// Member variables
	gsl::not_null<Window*> window_;
	Shader* shader_;
};