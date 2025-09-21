#include "scene.hpp"

Scene::Scene() {
    AddSphere(new Sphere(float3(0.0f, 0.0f, 5.0f), 2.f));
    m_lights.push_back(new Light(float3(0.0f, 0.0f, 0.f), float3(10.0f, 10.0f, 10.0f)));
}

Scene::~Scene() {
    for (GeometricObject* object : m_objects) {
        delete object;
    }
    m_objects.clear();

    for (Light* light: m_lights) {
        delete light;
    }
    m_lights.clear();
}

void Scene::AddSphere(Sphere* sphere)
{
    m_objects.push_back(sphere);
    m_bbox = AABB(m_bbox, sphere->GetBoundingBox());
}