#pragma once
#include "../scene/scene.hpp"

struct Ray {
	Ray() = default;
	Ray(float3 O, float3 D);

	float3 origin;
	float3 direction;
	float t = -1;
};

inline Ray::Ray(float3 O, float3 D) : origin(O), direction(D) {}

struct IntersectionPoint {
	IntersectionPoint() = default;
	IntersectionPoint(float3 p);

	float3 point;
	float3 normal;
};

inline IntersectionPoint::IntersectionPoint(float3 p) : point(p) {}

class RayTracer {
public:
	RayTracer() = default;
	Color GetBackgroundColor(const Ray& primaryRay);
	Color TraceRay(Ray& primaryRay, const Scene& scene);
	bool IntersectRaySphere(Ray& ray, const Sphere& sphere);
};