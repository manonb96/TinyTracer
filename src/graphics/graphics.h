#pragma once

#include "glfw/glfw_window.h"
#include "vertex.h"

class Graphics {
public:
	Graphics(gsl::not_null<Window*> window);
	virtual ~Graphics() = default;

	void Init();

	virtual void CreateVertexBuffer(gsl::span<Vertex> vertices) = 0;
	virtual void CreateIndexBuffer(gsl::span<int> indices) = 0;
	virtual void CreateTexture() = 0;
	virtual void RenderIndexedBuffer(unsigned char* pixels, unsigned int shaderID) = 0;

protected:
	virtual void Initialize() = 0;

	// Member variables
	gsl::not_null<Window*> window_;
};