#pragma once
#include "../utils/base_types.hpp"
#include "../raytracing/acceleration_structures/aabb.hpp"
#include "../raytracing/intersection_point.hpp"

class GeometricObject {
public:
	virtual bool Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const = 0;
	AABB GetBoundingBox() const;
	Color GetColor() const;

protected:
	Color m_color = Color(1, 0, 0);
	AABB m_bbox;
};

class Sphere : public GeometricObject { 
public: 
	Sphere(float3 c, float r);
	~Sphere() = default;

	bool Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const override;
	
private:
	float3 m_center;
	float m_radius;
};