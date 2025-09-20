#pragma once

#include "../../utils/interval.hpp"
#include "../ray.hpp"

class AABB {
public:
	AABB() = default;
	AABB(const Interval& x, const Interval& y, const Interval& z);
	AABB(const float3& a, const float3& b);
	~AABB() = default;

	bool IntersectRayAABB(const Ray& ray, Interval& rayInterval) const;

private:
	Interval m_x, m_y, m_z;

	Interval GetAxis(int n) const;
};