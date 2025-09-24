#include "scene.hpp"

Scene::Scene() {
    AddPrimitive(std::make_shared<Sphere>(float3(-3.f, 0.0f, 4.0f), 1.f));
    AddPrimitive(std::make_shared<Sphere>(float3(0.0f, 0.0f, 4.0f), 1.f, COLOR_GREEN));
    AddPrimitive(std::make_shared<Sphere>(float3(3.f, 0.0f, 4.0f), 1.f, COLOR_BLUE));
    AddPrimitive(std::make_shared<Sphere>(float3(0.f, -3.0f, 4.0f), 1.f, COLOR_LIGHTBLUE));
    AddPrimitive(std::make_shared<Sphere>(float3(0.f, 3.0f, 4.0f), 1.f, COLOR_WHITE));
    m_lights.push_back(new Light(float3(0.0f, 0.0f, 0.f), float3(20.0f, 20.0f, 20.0f)));
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