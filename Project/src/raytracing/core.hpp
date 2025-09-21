#pragma once

#include "../scene/camera.hpp"
#include "../scene/scene.hpp"
#include "raytracer.hpp"
#include "acceleration_structures/bvh.hpp"

class Core {
public: 
	Core();
	~Core();
	void GetPixels(uchar* pixels);

private:
	Camera* m_pMainCamera = nullptr;
	Scene* m_pScene = nullptr;

	BVHNode* m_pBVHTree = nullptr;
	RayTracer* m_pRayTracer = nullptr;

	void InitializeCore();
};