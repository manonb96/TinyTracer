#pragma once
#include "geometry.hpp"
#include "lights.hpp"
#include "../raytracing/acceleration_structures/aabb.hpp"

class Scene {
public:
	Scene();
	~Scene();

	const vector<GeometricObject*>& GetObjects() const { return m_objects; }
	const vector<Light*>& GetLights() const { return m_lights;  }

private:
	vector<GeometricObject*> m_objects;
	vector<Light*> m_lights;

	AABB m_bbox;

	void AddSphere(Sphere* sphere);
};