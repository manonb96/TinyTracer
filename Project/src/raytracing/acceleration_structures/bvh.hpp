#pragma once

#include "aabb.hpp"
#include "../../scene/scene.hpp"
#include "../../scene/geometry.hpp"

class BVHNode : public GeometricObject {
public:
	BVHNode(gsl::not_null<Scene*> scene);
	BVHNode(vector<GeometricObject*> objects, int start, int end);
	~BVHNode();

	bool Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const override;
private:
	GeometricObject* m_pLeft = nullptr;
	GeometricObject* m_pRight = nullptr;

	static bool BoxCompare(const GeometricObject* a, const GeometricObject* b, int axis)
	{
		Interval aAxisInterval = a->GetBoundingBox().GetAxis(axis);
		Interval bAxisInterval = b->GetBoundingBox().GetAxis(axis);
		return aAxisInterval.min < bAxisInterval.min;
	}

	static bool BoxCompare_X(const GeometricObject* a, const GeometricObject* b) {
		return BoxCompare(a, b, 0);
	}
	
	static bool BoxCompare_Y(const GeometricObject* a, const GeometricObject* b) {
		return BoxCompare(a, b, 1);
	}
	
	static bool BoxCompare_Z(const GeometricObject* a, const GeometricObject* b) {
		return BoxCompare(a, b, 2);
	}
};

