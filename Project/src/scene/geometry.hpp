#pragma once
#include "../utils/base_types.hpp"

struct Sphere { 
	Sphere(float3 c, float r) : center(c), radius(r) {}
	~Sphere() = default;
	
	float3 center;
	float radius;
	Color color = Color(1, 0, 0);
	// TODO: Add front_face
};