#pragma once
#include "base_types.hpp"

struct Ray {
	Ray() = default;
	Ray(float3 O, float3 D);

	float3 origin;
	float3 direction;
	float t = -1;
};

inline Ray::Ray(float3 O, float3 D) : origin(O), direction(D) {}