#pragma once
#include "../utils/base_types.hpp"
#include "raytracer.hpp"

class Sphere {
public: 
	Sphere(float3 c, float r);
	~Sphere() = default;
	float3 center;
	float radius;
	Color color = Color(1, 0, 0);

	bool IntersectRaySphere(Ray& ray);
};