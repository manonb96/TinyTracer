#pragma once
#include "raytracer.hpp"

class Camera {
public:
	Camera(float3 position);
	~Camera() = default;

	Ray GeneratePrimaryRay(int x, int y);
private:
	float3 cameraCenter;
	float3 right;
	float3 up;
	float3 forward;

	float3 pixelU;
	float3 pixelV;
	float3 pixelTopLeft;

	float aspectRatio;
	float focalLength;

	void PrecomputeImagePlane();
};