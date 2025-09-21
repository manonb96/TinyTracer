#pragma once
#include "geometry.hpp"

class Primitive : public GeometricObject {
public:
	Primitive(float3 position, Color albedo);
	~Primitive() = default;

	Color GetAlbedo() const;
	void SetID(uint id);
	uint GetID() const;

protected:
	float3 m_center;
	Color m_albedo;
	uint m_id = -1;
};

class Sphere : public Primitive {
public:
	Sphere(float3 center, float radius, Color albedo = COLOR_RED);
	~Sphere() = default;

	bool Hit(const Ray& ray, const Interval& ray_t, IntersectionPoint& intersectionPoint) const override;

private:
	float m_radius;
};