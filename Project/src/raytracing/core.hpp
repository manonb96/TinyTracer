#pragma once

#include "camera.hpp"
#include "../scene/scene.hpp"

class Core {
public: 
	Camera* mainCamera = nullptr;
	Scene* scene = nullptr;
	RayTracer* rayTracer = nullptr;
	
	Core() = default;
	~Core();
	void InitializeScene();
	void GetPixels(uchar* pixels);
};