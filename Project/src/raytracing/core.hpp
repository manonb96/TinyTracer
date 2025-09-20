#pragma once

#include "../scene/camera.hpp"
#include "../scene/scene.hpp"
#include "raytracer.hpp"

class Core {
public: 
	Core();
	~Core();
	void GetPixels(uchar* pixels);

private:
	Camera* m_pMainCamera = nullptr;
	Scene* m_pScene = nullptr;
	RayTracer* m_pRayTracer = nullptr;

	void InitializeCore();
};