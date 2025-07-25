#pragma once
#include <math.h>
#include <cmath>

struct int2 {
	int x, y;
	int2();
	int2(int a, int b);
};
inline int2::int2()	: x(0), y(0) {}
inline int2::int2(int a, int b) : x(a), y(b) {}
inline int2 operator+(int2 a, int2 b) { return int2(a.x + b.x, a.y + b.y); }
inline int2 operator-(int2 a, int2 b) { return int2(a.x - b.x, a.y - b.y); }
inline int2 operator/(int2 a, int b) { return int2(a.x / b, a.y / b); }

struct float2 {
	float x, y;
	float2();
	float2(float a, float b);
};
inline float2::float2() : x(0.0f), y(0.0f) {}
inline float2::float2(float a, float b) : x(a), y(b) {}

struct float3 {
	float x, y, z;
	float3();
	float3(float a, float b, float c);
};
inline float3::float3() : x(0.0f), y(0.0f), z(0.0f) {}
inline float3::float3(float a, float b, float c) : x(a), y(b), z(c) {}
inline float rsqrtf(float x) { return 1.0f / sqrtf(x); }
inline float3 operator+(float3 a, float3 b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline float3 operator-(float3 a, float3 b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline float3 operator*(float3 a, float b) { return float3(a.x * b, a.y * b, a.z * b); }
inline float3 operator*(float a, float3 b) { return float3(a * b.x, a * b.y, a * b.z); }
inline float3 operator*(float3 a, float3 b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline float3& operator+=(float3& a, const float3& b) {
	a.x += b.x; a.y += b.y; a.z += b.z; return a;
}
inline float3& operator-=(float3& a, const float3& b) {
	a.x -= b.x; a.y -= b.y; a.z -= b.z; return a;
}
inline float3& operator-(float3 a) {
	a.x = -a.x; a.y = -a.y; a.z = -a.z; return a;
}
inline float dot(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float length(float3 v) { return sqrtf(dot(v, v)); }
inline float3 normalize(float3 v) { float invLen = rsqrtf(dot(v, v)); return v * invLen; }
inline float3 cross(float3 a, float3 b) { return float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
inline float isNormal(float3 a) { return (fabsf(length(a) - 1.0f)) < 0.001f; }
