#include "scene.hpp"

Scene::Scene() {
    spheres.push_back(new Sphere(float3(0.0f, 0.0f, 5.0f), 2.f));
    lights.push_back(new Light(float3(0.0f, 0.0f, 2.f), float3(1.0f, 1.0f, 1.0f)));
    backgroundColor = Color(0.0f, 0.0f, 0.0f);
}

Scene::~Scene() {
    for (Sphere* obj : spheres)
        delete obj;
    spheres.clear();
    for (Light* obj : lights)
        delete obj;
    lights.clear();
}