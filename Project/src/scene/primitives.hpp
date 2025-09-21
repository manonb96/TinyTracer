#pragma once
#include "geometry.hpp"

class Primitive : public GeometricObject {
public:
	Primitive(float3 position);
	~Primitive() = default;

	Color GetColor() const;
	void SetID(uint id);
	uint GetID() const;

protected:
	float3 m_center;
	Color m_color = Color(1, 0, 0);
	uint m_id = -1;
};

class Sphere : public Primitive {
public:
	Sphere(float3 center, float radius);
	~Sphere() = default;

	bool Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const override;

private:
	float m_radius;
};