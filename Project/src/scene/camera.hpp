#pragma once
#include "../raytracing/ray.hpp"

class Camera {
public:
	Camera(float3 position);
	~Camera() = default;

	Ray GeneratePrimaryRay(float x, float y);
private:
	float3 m_cameraCenter;

	float3 m_pixelU;
	float3 m_pixelV;
	float3 m_pixelTopLeft;

	float m_aspectRatio;
	float m_focalLength;

	void PrecomputeImagePlane(const float3& right, const float3& up, const float3& forward);
};