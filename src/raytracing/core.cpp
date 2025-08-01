#include "core.hpp"
#include "../utils/constants.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "lights.hpp"
#include "raytracer.hpp"
#include <iostream>

void Core::InitializeScene() {
	mainCamera = new Camera();
    spheres.push_back(new Sphere(float3(-5.0f, 5.0f, 10.f), 2.f));
    lights.push_back(new Light(float3(-3.f, 2.f, 8.f), float3(1.0f, 1.0f, 1.0f)));
    backgroundColor = float3(0, 0, 0);
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

void Core::RenderScene(unsigned char* pixels) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Ray* primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            float3 color = backgroundColor;
            Sphere* nearestObject = nullptr;
            float tNear = INFINITY;

            // Find the closest sphere
            for (Sphere* sphere : spheres) {
                bool hit = sphere->IntersectRaySphere(*primaryRay);
                if (hit && primaryRay->t < tNear) {
                    nearestObject = sphere;
                    tNear = primaryRay->t;
                }
            }

            // Cast the shadow rays
            if (nearestObject != nullptr) {
                IntersectionPoint* ip = new IntersectionPoint(float3(primaryRay->origin + primaryRay->direction * tNear));
                ip->normal = normalize(ip->point - nearestObject->center);
                for (Light* light : lights) {
                    Ray* shadowRay = new Ray(ip->point + OFFSET * ip->normal, normalize(light->position - ip->point));
                    bool occluded = false;
                    float distanceToLight = length(light->position - ip->point);
                    for (Sphere* sphere : spheres) {
                        bool hit = sphere->IntersectRaySphere(*shadowRay);
                        if (hit && shadowRay->t < distanceToLight) {
                            occluded = true;
                            break;
                        }
                    }

                    if (!occluded) {
                        float3 L = normalize(light->position - ip->point);
                        float clampedCosTheta = std::max(dot(ip->normal, L), 0.f);
                        float attenuation = 1.0f / (distanceToLight * distanceToLight);
                        float3 diffuseColor = light->color * nearestObject->color * clampedCosTheta * attenuation;
                        color += attenuation * nearestObject->color;
                    }
                    delete shadowRay;
                }
                delete ip;
            }
            delete primaryRay;

            int offset = (y * WIDTH + x) * 3;
            pixels[offset + 0] = (int)(color.x * 255.f);
            pixels[offset + 1] = (int)(color.y * 255.f);
            pixels[offset + 2] = (int)(color.z * 255.f);
        }
    }
}