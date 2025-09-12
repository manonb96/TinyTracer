#include "core.hpp"
#include "raytracer.hpp"

void Core::InitializeScene() {
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
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Ray primaryRay = mainCamera->GeneratePrimaryRay(x, y);
            Color color = rayTracer->TraceRay(primaryRay, *scene);            
            WriteColor(pixels, x, y, color);
        }
    }
}