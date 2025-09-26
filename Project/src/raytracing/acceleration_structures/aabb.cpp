#include "aabb.hpp"

AABB::AABB(const Interval& x, const Interval& y, const Interval& z) :
	m_x(x), m_y(y), m_z(z) { }

AABB::AABB(const float3& pointA, const float3& pointB) {
	m_x = (pointA.x <= pointB.x) ? Interval(pointA.x, pointB.x) : Interval(pointB.x, pointA.x);
	m_y = (pointA.y <= pointB.y) ? Interval(pointA.y, pointB.y) : Interval(pointB.y, pointA.y);
	m_z = (pointA.z <= pointB.z) ? Interval(pointA.z, pointB.z) : Interval(pointB.z, pointA.z);
}

AABB::AABB(const AABB& boxA, const AABB& boxB) {
	m_x = Interval(boxA.GetAxis(0), boxB.GetAxis(0));
	m_y = Interval(boxA.GetAxis(1), boxB.GetAxis(1));
	m_z = Interval(boxA.GetAxis(2), boxB.GetAxis(2));
}

Interval AABB::GetAxis(int n) const {
	if (n == 1) return m_y;
	if (n == 2) return m_z;
	return m_x;
}

int AABB::GetLongestAxis() const {
	if (m_x.Size() > m_y.Size()) {
		return m_x.Size() > m_z.Size() ? 0 : 2;
	}
	return m_y.Size() > m_z.Size() ? 1 : 2;
}

bool AABB::IntersectRayAABB(const Ray& ray, const Interval& rayInterval) const {
#ifdef STATS
	stats->aabbRayIntersectionCounter++;
#endif

	float min = rayInterval.min;
	float max = rayInterval.max;

	for (int axis = 0; axis < 3; axis++)
	{
		const Interval& intervalForAxis = GetAxis(axis);
		const float inverseDirection = 1.0f / ray.direction[axis];

		float t0 = (intervalForAxis.min - ray.origin[axis]) * inverseDirection;
		float t1 = (intervalForAxis.max - ray.origin[axis]) * inverseDirection;

		if (t0 > t1) {
			std::swap(t0, t1);
		}

		min = std::max(min, t0);
		max = std::min(max, t1);

		if (max <= min)
			return false;
	}

	return true;
}