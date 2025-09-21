#pragma once
#include "../utils/base_types.hpp"
#include "../raytracing/acceleration_structures/aabb.hpp"
#include "../raytracing/intersection_point.hpp"

class GeometricObject {
public:
	virtual bool Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const = 0;
	AABB GetBoundingBox() const;

protected:
	AABB m_bbox;
};

inline AABB GeometricObject::GetBoundingBox() const {
	return m_bbox;
}