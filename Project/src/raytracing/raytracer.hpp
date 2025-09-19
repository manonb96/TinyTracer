#pragma once
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"
#include "ray.hpp"

struct IntersectionPoint {
	IntersectionPoint() = default;
	IntersectionPoint(float3 p);

	float3 point;
	float3 normal;
};

inline IntersectionPoint::IntersectionPoint(float3 p) : point(p) {}

class RayTracer {
public:
	RayTracer(int spp = 1);
	Color GetPixelColor(int x, int y, Camera& camera, const Scene& scene);
private:
	Color GetBackgroundColor(const Ray& ray);
	Color TraceRay(Ray& primaryRay, const Scene& scene);
	bool IntersectRaySphere(Ray& ray, const Sphere& sphere);

	int samplesPerPixel;
	float samplesPerPixelScale;
};