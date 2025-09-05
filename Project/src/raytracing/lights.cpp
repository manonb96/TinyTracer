#include "lights.hpp"

Light::Light(float3 pos, float3 c) : position(pos), color(c) {}

float Light::DistanceToLight(const float3& intersectionPoint) {
	return length(position - intersectionPoint);
}