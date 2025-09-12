#include "core.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "lights.hpp"
#include "raytracer.hpp"
#include <iostream>

void Core::InitializeScene() {
	mainCamera = new Camera(float3(0.f, 0.f, 0.f));
    spheres.push_back(new Sphere(float3(0.0f, 0.0f, 5.0f), 2.f));
    lights.push_back(new Light(float3(0.0f, 0.0f, 2.f), float3(1.0f, 1.0f, 1.0f)));
    backgroundColor = Color(0.0f, 0.0f, 0.0f);
}

Core::~Core() {
	delete mainCamera;
    for (Sphere* obj : spheres)
        delete obj;
    spheres.clear();
    for (Light* obj : lights)
        delete obj;
    lights.clear();
}

void Core::RaytraceScene(uchar* pixels) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Ray primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            Color color = backgroundColor;

            // Find the closest sphere
            Sphere* nearestObject = nullptr;
            float tNear = INFINITY;
            for (Sphere* sphere : spheres) {
                bool hit = sphere->IntersectRaySphere(primaryRay);
                if (hit && primaryRay.t < tNear) {
                    nearestObject = sphere;
                    tNear = primaryRay.t;
                }
            }

            // Cast the shadow rays
            if (nearestObject != nullptr) {
                IntersectionPoint ip(float3(primaryRay.origin + primaryRay.direction * tNear));
                ip.normal = normalize(ip.point - nearestObject->center);
                for (Light* light : lights) {
                    Ray shadowRay(ip.point + OFFSET * ip.normal, normalize(light->position - ip.point));
                    bool occluded = false;
                    float distanceToLight = length(light->position - ip.point);
                    for (Sphere* sphere : spheres) {
                        bool hit = sphere->IntersectRaySphere(shadowRay);
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
						color.rgb += attenuation * nearestObject->color.rgb;
					}
                }
            }
            
            WriteColor(pixels, x, y, color);
        }
    }
}