#include "camera.hpp"

Camera::Camera(float3 position) : m_cameraCenter(position) {
	// Default orientation
	float3 right = float3(1.f, 0.f, 0.f);
	float3 up = float3(0.f, 1.f, 0.f);
	float3 forward = float3(0.f, 0.f, 1.f);
	
	// Default values
	m_aspectRatio = (float)IMAGE_WIDTH / (float)IMAGE_HEIGHT;
	m_focalLength = 1.0f;

	PrecomputeImagePlane(right, up, forward);
}

Ray Camera::GeneratePrimaryRay(float x, float y) {
	float3 pixelCenter = float3(m_pixelTopLeft.x + x * m_pixelU.x, m_pixelTopLeft.y + y * m_pixelV.y, m_pixelTopLeft.z);
	float3 direction = normalize(pixelCenter - m_cameraCenter);

	return Ray(m_cameraCenter, direction);
}

void Camera::PrecomputeImagePlane(const float3& right, const float3& up, const float3& forward) {
	float imagePlaneHeight = 2.0f;
	float imagePlaneWidth = imagePlaneHeight * m_aspectRatio;

	float3 imagePlaneCenter = m_cameraCenter + forward * m_focalLength;
	float3 imagePlaneTopleft = imagePlaneCenter - (right * 0.5f * imagePlaneWidth) - (up * 0.5f * imagePlaneHeight);

	m_pixelU = float3(imagePlaneWidth / (float)IMAGE_WIDTH, 0, 0);
	m_pixelV = float3(0, imagePlaneHeight / (float)IMAGE_HEIGHT, 0);
	m_pixelTopLeft = imagePlaneTopleft + 0.5f * m_pixelU + 0.5f * m_pixelV;
}