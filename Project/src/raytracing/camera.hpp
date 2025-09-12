#pragma once
#include "raytracer.hpp"

class Camera {
public:
	Camera(float3 position);
	Camera(float3 pos, float3 target, float3 up);
	~Camera() = default;
	float3 position;
	float3 right;
	float3 up;
	float3 forward;
	float aspectRatio;

	Ray* GeneratePrimaryRay(int x, int y);
};