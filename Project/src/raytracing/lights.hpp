#pragma once

class Light {
public: 
	Light(float3 pos, float3 c);
	~Light() = default;
	float3 position;
	float3 color;

	float DistanceToLight(const float3& intersectionPoint);
};