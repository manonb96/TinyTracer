#pragma once

#include "aabb.hpp"
#include "../../scene/scene.hpp"
#include "../../scene/geometry.hpp"

class BVHNode : public GeometricObject {
public:
	// BVHNode(gsl::not_null<Scene*> scene);
	BVHNode(vector<std::shared_ptr<Primitive>>& objects, int start, int end);
	~BVHNode();

	bool Hit(const Ray& ray, const Interval& ray_t, IntersectionPoint& intersectionPoint) const override;
private:
	std::shared_ptr<GeometricObject> m_pLeft = nullptr;
	std::shared_ptr<GeometricObject> m_pRight = nullptr;

	static bool BoxCompare(const std::shared_ptr<GeometricObject> a, const std::shared_ptr<GeometricObject> b, int axis)
	{
		Interval aAxisInterval = a->GetBoundingBox().GetAxis(axis);
		Interval bAxisInterval = b->GetBoundingBox().GetAxis(axis);
		return aAxisInterval.min < bAxisInterval.min;
	}

	static bool BoxCompare_X(const std::shared_ptr<GeometricObject> a, const std::shared_ptr<GeometricObject> b) {
		return BoxCompare(a, b, 0);
	}
	
	static bool BoxCompare_Y(const std::shared_ptr<GeometricObject> a, const std::shared_ptr<GeometricObject> b) {
		return BoxCompare(a, b, 1);
	}
	
	static bool BoxCompare_Z(const std::shared_ptr<GeometricObject> a, const std::shared_ptr<GeometricObject> b) {
		return BoxCompare(a, b, 2);
	}
};

