#include "raytracer.hpp"

RayTracer::RayTracer(int spp) : m_samplesPerPixel(spp) { 
    m_samplesPerPixelScale = 1.0f / spp;
}

Color RayTracer::GetPixelColor(int x, int y, Camera& camera, const Scene& scene) {
    Color color = COLOR_BLACK;
    
    for (int i = 0; i < m_samplesPerPixel; i++) 
    {
        float offsetX = GetRandomFloat() - 0.5f;
        float offsetY = GetRandomFloat() - 0.5f;
        Ray primaryRay = camera.GeneratePrimaryRay(x + offsetX, y + offsetY);
        color += TraceRay(primaryRay, scene);
    }

    color *= m_samplesPerPixelScale;
    return color;
}

Color RayTracer::TraceRay(Ray& primaryRay, const Scene& scene) {
    Color color = COLOR_BLACK;

    // Find the closest object
    GeometricObject* nearestObject = nullptr;
    IntersectionPoint nearestIntersectionPoint; 
    
    IntersectionPoint tmpIntersectionPoint;
    auto& objects = scene.GetObjects();
    for (GeometricObject* object : objects) {
        bool hit = object->Hit(primaryRay, k_rayLength, tmpIntersectionPoint);
        if (hit && tmpIntersectionPoint.t < nearestIntersectionPoint.t) {
            nearestObject = object;
            nearestIntersectionPoint = tmpIntersectionPoint;
        }
    }

    // Cast the shadow rays
    if (nearestObject != nullptr) {
       
        // Reset
        tmpIntersectionPoint.t = INFINITY;
        
        auto& lights = scene.GetLights();
        for (Light* light : lights) {
            Ray shadowRay(nearestIntersectionPoint.point + OFFSET * nearestIntersectionPoint.normal, normalize(light->position - nearestIntersectionPoint.point));
            bool occluded = false;
            float distanceToLight = length(light->position - nearestIntersectionPoint.point);
            for (GeometricObject* object : objects) {
                if (object == nearestObject) {
                    continue;
                }
                
                bool hit = object->Hit(shadowRay, k_rayLength, tmpIntersectionPoint);
                if (hit && tmpIntersectionPoint.t < distanceToLight) {
                    occluded = true;
                    break;
                }
            }

            if (!occluded) {
                float3 L = normalize(light->position - nearestIntersectionPoint.point);
                float clampedCosTheta = std::max(dot(nearestIntersectionPoint.normal, L), 0.f);
                float attenuation = 1.0f / (distanceToLight * distanceToLight);
                float3 diffuseColor = light->color * nearestObject->GetColor().rgb * clampedCosTheta * attenuation;
                color.rgb += diffuseColor;
            }
        }
    }
    else {
        color = GetBackgroundColor(primaryRay);
    }

    return color;
}

Color RayTracer::GetBackgroundColor(const Ray& primaryRay) {
    float a = 0.5f * (primaryRay.direction.y + 1.0f); 
    return Lerp(COLOR_WHITE, COLOR_LIGHTBLUE, 1.0f - a);
}