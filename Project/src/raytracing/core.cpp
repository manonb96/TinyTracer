#include <chrono>
#include "core.hpp"
#include "raytracer.hpp"

Core::Core() {
    InitializeCore();
}

Core::~Core() {
    delete m_pMainCamera;
    delete m_pScene;
    delete m_pRayTracer;
}

void Core::InitializeCore() {
	m_pMainCamera = new Camera(float3(0.f, 0.f, -2.f));
    m_pScene = new Scene();
    m_pRayTracer = new RayTracer(8);

#if BVH
    m_pRayTracer->BuildBVH(*m_pScene);
#endif
}

void Core::GetPixels(uchar* pixels) {
#ifdef STATS
    stats = new Stats();
#endif

    auto start = std::chrono::steady_clock::now();
    
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            Color color = m_pRayTracer->GetPixelColor(x, y, *m_pMainCamera, *m_pScene);            
            WriteColor(pixels, x, y, color);
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    spdlog::info("Timer | GetPixels: {}", duration.count());

#ifdef STATS
    spdlog::info("---------- Stats ----------", stats->sphereRayIntersectionCounter);
    spdlog::info("Primary rays generated: {}", stats->primaryRayCounter);
    spdlog::info("Sphere/Box intersection tests: {}", stats->aabbRayIntersectionCounter);
    spdlog::info("Sphere/Ray intersection tests: {}", stats->sphereRayIntersectionCounter);
    spdlog::info("---------------------------", stats->sphereRayIntersectionCounter);
    delete stats;
#endif
}