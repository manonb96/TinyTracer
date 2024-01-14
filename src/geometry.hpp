#pragma once
#include "base_types.hpp"
#include "raytracer.hpp"

class Ray;

class Sphere {
public: 
	Sphere(float3 c, float r);
	~Sphere() = default;
	float3 center;
	float radius;
	float3 color = float3(1.0f, 0.f, 0.f);

	bool IntersectRaySphere(Ray& ray);
};