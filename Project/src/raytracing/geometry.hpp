#pragma once
#include "../utils/base_types.hpp"
#include "raytracer.hpp"

class Ray;

class Sphere {
public: 
	Sphere(float3 c, float r);
	~Sphere() = default;
	float3 center;
	float radius;
	float3 color = float3(1, 0, 0);

	bool IntersectRaySphere(Ray& ray);
};