#pragma once

#include "../scene/camera.hpp"
#include "../scene/scene.hpp"
#include "raytracer.hpp"

class Core {
public: 
	Camera* mainCamera = nullptr;
	Scene* scene = nullptr;
	RayTracer* rayTracer = nullptr;
	
	Core() = default;
	~Core();
	void InitializeCore();
	void GetPixels(uchar* pixels);
};