#include "raytracer.hpp"

Color RayTracer::TraceRay(Ray& primaryRay, const Scene& scene) {
    Color color = COLOR_BLACK;

    // Find the closest sphere
    Sphere* nearestObject = nullptr;
    float tNear = INFINITY;
    for (Sphere* sphere : scene.spheres) {
        bool hit = IntersectRaySphere(primaryRay, *sphere);
        if (hit && primaryRay.t < tNear) {
            nearestObject = sphere;
            tNear = primaryRay.t;
        }
    }

    // Cast the shadow rays
    if (nearestObject != nullptr) {
        IntersectionPoint ip(float3(primaryRay.origin + primaryRay.direction * tNear));
        ip.normal = normalize(ip.point - nearestObject->center);

        for (Light* light : scene.lights) {
            Ray shadowRay(ip.point + OFFSET * ip.normal, normalize(light->position - ip.point));
            bool occluded = false;
            float distanceToLight = length(light->position - ip.point);
            for (Sphere* sphere : scene.spheres) {
                if (sphere == nearestObject) {
                    continue;
                }
                bool hit = IntersectRaySphere(shadowRay, *sphere);
                if (hit && shadowRay.t < distanceToLight) {
                    occluded = true;
                    break;
                }
            }

            if (!occluded) {
                float3 L = normalize(light->position - ip.point);
                float clampedCosTheta = std::max(dot(ip.normal, L), 0.f);
                float attenuation = 1.0f / (distanceToLight * distanceToLight);
                float3 diffuseColor = light->color * nearestObject->color.rgb * clampedCosTheta * attenuation;
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
    return lerp(Color(1.0, 1.0, 1.0), Color(0.5, 0.7, 1.0), 1.0f - a);
}

bool RayTracer::IntersectRaySphere(Ray& ray, const Sphere& sphere) {
    float3 L = sphere.center - ray.origin;
    float tca = dot(L, ray.direction);
    // if (tca < 0) return false;

    float r2 = sphere.radius * sphere.radius;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > r2) return false;

    float thc = sqrt(r2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 > t1) std::swap(t0, t1);
    if (t0 < 0) t0 = t1;

    ray.t = t0;

    if (t0 < 0) return false;
    return true;
}