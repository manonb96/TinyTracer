#include "geometry.hpp"
#include <algorithm>
#include "raytracer.hpp"

Sphere::Sphere(float3 c, float r) : center(c), radius(r) {}

bool Sphere::IntersectRaySphere(Ray& ray) {
	float3 L = center - ray.origin;
	float tca = dot(L, ray.direction);
	// if (tca < 0) return false;

	float r2 = radius * radius;
	float d2 = dot(L, L) - tca * tca;
	if (d2 > r2) return false;
	
	float thc = sqrt(r2 - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);
	if (t0 < 0) t0 = t1;

	ray.t = t0;

	if (t0 < 0) return false;
	return true;
}