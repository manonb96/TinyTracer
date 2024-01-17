#include "core.hpp"
#include "constants.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "raytracer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

void Core::InitializeScene() {
	mainCamera = new Camera();
	mainSphere = new Sphere(float3(0.f, 0.f, 10.f), 2.f);
}

Core::~Core() {
	delete mainCamera;
	delete mainSphere;
}

float3 Core::CalculateColor(bool hitWithMain) {
	if (hitWithMain) 
        return mainSphere->color;
	else return float3(0, 0, 0);
}

void Core::RenderScene(unsigned char* pixels) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Ray* primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            bool hit = mainSphere->IntersectRaySphere(*primaryRay);
            float3 color = CalculateColor(hit);

            int offset = (y * WIDTH + x) * 3;
            pixels[offset + 0] = color.x;
            pixels[offset + 1] = color.y;
            pixels[offset + 2] = color.z;
        }
    }
}