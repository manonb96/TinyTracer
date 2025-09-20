#include "scene.hpp"

Scene::Scene() {
    m_spheres.push_back(new Sphere(float3(0.0f, 0.0f, 5.0f), 2.f));
    m_lights.push_back(new Light(float3(0.0f, 0.0f, 0.f), float3(10.0f, 10.0f, 10.0f)));
}

Scene::~Scene() {
    for (Sphere* sphere : m_spheres) {
        delete sphere;
    }
    m_spheres.clear();

    for (Light* light: m_lights) {
        delete light;
    }
    m_lights.clear();
}