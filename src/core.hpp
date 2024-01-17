#pragma once
#include "base_types.hpp"

class Camera;
class Sphere;

class Core {
public: 
	Core() = default;
	~Core();
	Camera* mainCamera;
	Sphere* mainSphere;
	void InitializeScene();
	void RenderScene(unsigned char* pixels);
private:
	float3 CalculateColor(bool hitWithMain);
};