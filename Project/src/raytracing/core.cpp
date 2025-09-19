#include <chrono>
#include "core.hpp"
#include "raytracer.hpp"

void Core::InitializeCore() {
	mainCamera = new Camera(float3(0.f, 0.f, 0.f));
    scene = new Scene();
    rayTracer = new RayTracer();
}

Core::~Core() {
	delete mainCamera;
    delete scene;
    delete rayTracer;
}

void Core::GetPixels(uchar* pixels) {
    auto start = std::chrono::steady_clock::now();
    
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            Ray primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            Color color = rayTracer->TraceRay(primaryRay, *scene);            
            WriteColor(pixels, x, y, color);
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Timer | GetPixels: {}", duration.count());
}