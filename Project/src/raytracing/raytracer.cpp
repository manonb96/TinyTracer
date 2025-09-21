#include "raytracer.hpp"

RayTracer::RayTracer(int spp) : m_samplesPerPixel(spp) { 
    m_samplesPerPixelScale = 1.0f / spp;
}

RayTracer::~RayTracer() {
    delete m_pBVHTree;
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

void RayTracer::BuildBVH(const Scene& scene)
{
    auto start = std::chrono::steady_clock::now();
    auto objectsCopy = scene.GetObjectsCopy();
    m_pBVHTree = new BVHNode(objectsCopy, 0, objectsCopy.size());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Timer | Build BVH: {}", duration.count());
}

Color RayTracer::TraceRay(Ray& primaryRay, const Scene& scene) {
    Color color = COLOR_BLACK;
    Interval defaultRayLength = Interval(0, INFINITY);

    // Find the closest object
    IntersectionPoint nearestIntersectionPoint; 
    
    IntersectionPoint tmpIntersectionPoint;
    auto& objects = scene.GetObjects();

#if BVH
    bool hit = m_pBVHTree->Hit(primaryRay, defaultRayLength, tmpIntersectionPoint);
    if (hit && tmpIntersectionPoint.t < nearestIntersectionPoint.t) {
        nearestIntersectionPoint = tmpIntersectionPoint;
    }
#else
    for (int i = 0; i < objects.size(); i++) {
        std::shared_ptr<GeometricObject> object = objects[i];
        bool hit = object->Hit(primaryRay, defaultRayLength, tmpIntersectionPoint);
        if (hit && tmpIntersectionPoint.t < nearestIntersectionPoint.t) {
            nearestIntersectionPoint = tmpIntersectionPoint;
        }
    }
#endif 

    // Cast the shadow rays
    if (nearestIntersectionPoint.objectID != -1) {
       
        // Reset
        tmpIntersectionPoint.t = INFINITY;
        
        auto& lights = scene.GetLights();
        for (Light* light : lights) {
            Ray shadowRay(nearestIntersectionPoint.point + OFFSET * nearestIntersectionPoint.normal, normalize(light->position - nearestIntersectionPoint.point));
            bool occluded = false;
            float distanceToLight = length(light->position - nearestIntersectionPoint.point);
#if BVH
            bool hit = m_pBVHTree->Hit(shadowRay, defaultRayLength, tmpIntersectionPoint);
            if (hit && nearestIntersectionPoint.objectID != tmpIntersectionPoint.objectID && tmpIntersectionPoint.t < distanceToLight) {
                nearestIntersectionPoint = tmpIntersectionPoint;
            }
#else
            for (int i = 0; i < objects.size(); i++) {
                if (i == nearestIntersectionPoint.objectID) {
                    continue;
                }

                std::shared_ptr<GeometricObject> object = objects[i];
                bool hit = object->Hit(shadowRay, defaultRayLength, tmpIntersectionPoint);
                if (hit && tmpIntersectionPoint.t < distanceToLight) {
                    occluded = true;
                    break;
                }
            }
#endif

            if (!occluded) {
                float3 L = normalize(light->position - nearestIntersectionPoint.point);
                float clampedCosTheta = std::max(dot(nearestIntersectionPoint.normal, L), 0.f);
                float attenuation = 1.0f / (distanceToLight * distanceToLight);
                float3 diffuseColor = light->color * objects[nearestIntersectionPoint.objectID]->GetAlbedo().rgb * clampedCosTheta * attenuation;
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