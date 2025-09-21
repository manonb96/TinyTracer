#pragma once

#include "../../utils/interval.hpp"
#include "../ray.hpp"

class AABB {
public:
	AABB() = default;
	AABB(const Interval& x, const Interval& y, const Interval& z);
	AABB(const float3& pointA, const float3& pointB);
	AABB(const AABB& boxA, const AABB& boxB);
	~AABB() = default;

	bool IntersectRayAABB(const Ray& ray, Interval& rayInterval) const;
	Interval GetAxis(int n) const;

private:
	Interval m_x, m_y, m_z;
};