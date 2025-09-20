#pragma once
#include "../raytracing/ray.hpp"

class Camera {
public:
	Camera(float3 position, float3 lookAt = float3(0.f, 0.f, 1.0f), float3 worldUp = float3(0.f, 1.f, 0.f));
	~Camera() = default;

	Ray GeneratePrimaryRay(float x, float y);
private:
	float3 m_cameraCenter;

	float3 m_pixelU;
	float3 m_pixelV;
	float3 m_pixelTopLeft;

	float m_aspectRatio;
	float m_focalLength;
	float m_fieldOfView; // vertical view angle

	void PrecomputeImagePlane(const float3& right, const float3& up, const float3& forward);
};