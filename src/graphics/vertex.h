#pragma once

#include "../utils/precomp.h"

struct Vertex {
	Vertex() : position(float3(0.0f)), uv(float2(0.0f)) {}
	Vertex(float3 _position, float2 _uv) : position(_position), uv(_uv) {}

	float3 position;
	float2 uv;
};