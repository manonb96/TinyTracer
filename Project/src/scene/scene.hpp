#pragma once
#include "geometry.hpp"
#include "lights.hpp"

class Scene {
public:
	Scene();
	~Scene();

	vector<Sphere*> spheres;
	vector<Light*> lights;
	Color backgroundColor;
};