#pragma once
#include "../utils/base_types.hpp"
#include <vector>

class Camera;
class Sphere;
class Light;

class Core {
public: 
	Core() = default;
	~Core();
	Camera* mainCamera;
	vector<Sphere*> spheres;
	vector<Light*> lights;
	float3 backgroundColor;
	void InitializeScene();
	void RenderScene(uchar* pixels);
};