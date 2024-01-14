#pragma once
#include "base_types.hpp"
#include <GL/glew.h>

class Camera;
class Sphere;

class Core {
public: 
	Core() = default;
	~Core();
	Camera* mainCamera;
	Sphere* mainSphere;
	void InitializeScene();
	void InitializeGLEW();
	void InitializeFramebuffer();
	void RenderScene();
private:
	GLuint framebuffer;
	float3 CalculateColor(bool hitWithMain);
};