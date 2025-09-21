#pragma once

struct IntersectionPoint {
	IntersectionPoint() = default;
	IntersectionPoint(float3 p) : point(p) {}

	float3 point;
	float3 normal;
	float t = INFINITY;
	int objectID = -1;
};