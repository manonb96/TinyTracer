#pragma once

struct Ray {
	Ray() = default;
	Ray(float3 O, float3 D) : origin(O), direction(D) {}

	float3 origin;
	float3 direction;
	float t = -1;
};
