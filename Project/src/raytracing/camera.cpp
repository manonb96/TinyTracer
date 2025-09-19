#include "camera.hpp"

Camera::Camera(float3 position) : cameraCenter(position)
{
	// Default orientation
	right = float3(1.f, 0.f, 0.f);
	up = float3(0.f, 1.f, 0.f);
	forward = float3(0.f, 0.f, 1.f);
	
	aspectRatio = (float)IMAGE_WIDTH / (float)IMAGE_HEIGHT;
	focalLength = 1.0f;

	PrecomputeImagePlane();
}

Ray Camera::GeneratePrimaryRay(int x, int y)
{
	float3 pixelCenter = float3(pixelTopLeft.x + x * pixelU.x, pixelTopLeft.y + y * pixelV.y, pixelTopLeft.z);
	float3 direction = normalize(pixelCenter - cameraCenter);

	return Ray(cameraCenter, direction);
}

void Camera::PrecomputeImagePlane() {
	float imagePlaneHeight = 2.0f;
	float imagePlaneWidth = imagePlaneHeight * aspectRatio;

	float3 imagePlaneCenter = cameraCenter + forward * focalLength;
	float3 imagePlaneTopleft = imagePlaneCenter - (right * 0.5f * imagePlaneWidth) - (up * 0.5f * imagePlaneHeight);

	pixelU = float3(imagePlaneWidth / (float)IMAGE_WIDTH, 0, 0);
	pixelV = float3(0, imagePlaneHeight / (float)IMAGE_HEIGHT, 0);
	pixelTopLeft = imagePlaneTopleft + 0.5f * pixelU + 0.5f * pixelV;
}