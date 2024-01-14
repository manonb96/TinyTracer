#include "camera.hpp"
#include "raytracer.hpp"
#include "constants.hpp"

Camera::Camera() {
	position = float3(0.f, 0.f, 0.f);
	right = float3(1.f, 0.f, 0.f);
	up = float3(0.f, 1.f, 0.f);
	forward = float3(0.f, 0.f, 1.f);
	aspectRatio = WIDTH / HEIGHT;
}

Camera::Camera(float3 pos, float3 target, float3 up) {
	position = pos;
	forward = normalize(target - pos);
	right = normalize(cross(forward, up));
	up = cross(right, forward);
	aspectRatio = WIDTH / HEIGHT;
}

Ray* Camera::GeneratePrimaryRay(float x, float y)
{
	float x_ndc = (x + 0.5f) / WIDTH;
	float y_ndc = (y + 0.5f) / HEIGHT;

	float x_screen = ((2.0f * x_ndc) - 1.0f) * aspectRatio;
	float y_screen = 1.0f - (2.0f * y_ndc);

	float3 n = normalize(forward + x_screen * right + y_screen * up);
	return new Ray(position, n);
}