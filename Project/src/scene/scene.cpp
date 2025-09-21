#include "scene.hpp"

Scene::Scene() {
    AddPrimitive(std::make_shared<Sphere>(float3(0.0f, 0.0f, 5.0f), 2.f));
    m_lights.push_back(new Light(float3(0.0f, 0.0f, 0.f), float3(10.0f, 10.0f, 10.0f)));
}

Scene::~Scene() {
    m_objects.clear();

    for (Light* light: m_lights) {
        delete light;
    }
    m_lights.clear();
}

void Scene::AddPrimitive(std::shared_ptr<Primitive> primitiveObject)
{
    primitiveObject->SetID(primitiveCount);
    m_objects.push_back(primitiveObject);
    m_bbox = AABB(m_bbox, primitiveObject->GetBoundingBox());

    primitiveCount++;
}