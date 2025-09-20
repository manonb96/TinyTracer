#pragma once
#include "geometry.hpp"
#include "lights.hpp"

class Scene {
public:
	Scene();
	~Scene();

	const vector<Sphere*>& GetSpheres() const { return m_spheres; }
	const vector<Light*>& GetLights() const { return m_lights;  }

private:
	vector<Sphere*> m_spheres;
	vector<Light*> m_lights;
};