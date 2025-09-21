#pragma once

struct Ray {
	Ray() = default;
	Ray(float3 O, float3 D) : origin(O), direction(D) {}

	float3 origin;
	float3 direction;
	float t = -1;

	float3 GetPoint(float t) const;
};

inline float3 Ray::GetPoint(float t) const {
	return origin + t * direction;
}
