#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "glfw/glfw_window.h"
#include "vertex.h"
#include "uniform_transformations.h"
#include "../utils/precomp.h"
#include "../shaders/shader.hpp"

class Graphics {
public:
	Graphics(gsl::not_null<Window*> window, Shader* shader);
	virtual ~Graphics() = default;

	void Init();

	virtual bool BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void CreateVertexBuffer(gsl::span<Vertex> vertices) = 0;
	virtual void CreateIndexBuffer(gsl::span<int> indices) = 0;
	virtual void SetViewProjection(glm::mat4 view, glm::mat4 projection) = 0;
	virtual void CreateTexture(unsigned char* pixels) = 0;
	virtual void RenderIndexedBuffer() = 0;

protected:
	virtual void Initialize() = 0;

	// Member variables
	gsl::not_null<Window*> window_;
	Shader* shader_;
};