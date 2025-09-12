#pragma once
#include <vector>

class Camera;
class Sphere;
class Light;

class Core {
public: 
	Core() = default;
	~Core();
	Camera* mainCamera = nullptr;
	vector<Sphere*> spheres;
	vector<Light*> lights;
	float3 backgroundColor;
	void InitializeScene();
	void RenderScene(uchar* pixels);
};