#pragma once

struct Light {
	Light(float3 pos, float3 c) : position(pos), color(c) {}
	~Light() = default;
	float3 position;
	float3 color;

	float DistanceToLight(const float3& point);
};

inline float Light::DistanceToLight(const float3& intersectionPoint) {
	return length(position - intersectionPoint);
}