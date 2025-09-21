#include "bvh.hpp"
#include <algorithm>

BVHNode::BVHNode(vector<std::shared_ptr<Primitive>>& objects, int start, int end) {
	m_bbox = AABB_EMPTY;
	for (int object_index = start; object_index < end; object_index++) 	{
		m_bbox = AABB(m_bbox, objects[object_index]->GetBoundingBox());
	}

	int axis = m_bbox.GetLongestAxis();
	int objectCount = end - start;

	// Leaf
	if (objectCount == 1) {
		m_pLeft = objects[start];
		m_bbox = m_pLeft->GetBoundingBox();
		return;
	}	

	if (objectCount == 2) {
		m_pLeft = objects[start];
		m_pRight = objects[start + 1];
	}
	else {
		auto comparator = (axis == 0) ? BoxCompare_X
			: (axis == 1) ? BoxCompare_Y
			: BoxCompare_Z;
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		int mid = objectCount / 2;
		m_pLeft = std::make_shared<BVHNode>(objects, start, mid);
		m_pRight = std::make_shared<BVHNode>(objects, mid, end);
	}

	m_bbox = AABB(m_pLeft->GetBoundingBox(), m_pRight->GetBoundingBox());
}

bool BVHNode::Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const {
	if (!m_bbox.IntersectRayAABB(ray, ray_t)) {
		return false;
	}

	bool hitLeft = m_pLeft->Hit(ray, ray_t, intersectionPoint);
	bool hitRight = false;
	
	if (m_pRight != nullptr) {
		hitRight = m_pRight->Hit(ray, ray_t, intersectionPoint);
	}

	return hitLeft || hitRight;
}

BVHNode::~BVHNode() { }