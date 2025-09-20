#include "aabb.hpp"

AABB::AABB(const Interval& x, const Interval& y, const Interval& z) :
	m_x(x), m_y(y), m_z(z) { }

AABB::AABB(const float3& a, const float3& b) {
	m_x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
	m_y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
	m_z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);
}

Interval AABB::GetAxis(int n) const {
	if (n == 1) return m_y;
	if (n == 2) return m_z;
	return m_x;
}

bool AABB::IntersectRayAABB(const Ray& ray, Interval& rayInterval) const {
	for (int axis = 0; axis < 3; axis++)
	{
		const Interval& intervalForAxis = GetAxis(axis);
		const float inverseDirection = 1.0f / ray.direction[axis];

		float t0 = (intervalForAxis.min - ray.origin[axis]) * inverseDirection;
		float t1 = (intervalForAxis.max - ray.origin[axis]) * inverseDirection;

		if (t0 > t1) {
			std::swap(t0, t1);
		}

		if (t0 > rayInterval.min) {
			rayInterval.min = t0;
		}
		if (t1 < rayInterval.max) {
			rayInterval.max = t1;
		}

		if (rayInterval.max <= rayInterval.min)
			return false;
	}
	return true;
}