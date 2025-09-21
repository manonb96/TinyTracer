#pragma once
#include "primitives.hpp"
#include "lights.hpp"
#include "../raytracing/acceleration_structures/aabb.hpp"

class Scene {
public:
	Scene();
	~Scene();

	const vector<Light*>& GetLights() const { return m_lights; }
	const vector<std::shared_ptr<Primitive>>& GetObjects() const { return m_objects; }

	vector<std::shared_ptr<Primitive>> GetObjectsCopy() const { return m_objects;  }
	int GetObjectsCount() const { return m_objects.size(); }

private:
	vector<std::shared_ptr<Primitive>> m_objects;
	vector<Light*> m_lights;

	AABB m_bbox;
	uint primitiveCount = 0;

	void AddPrimitive(std::shared_ptr<Primitive> primitiveObject);
};