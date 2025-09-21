#pragma once
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"
#include "ray.hpp"

class RayTracer {
public:
	RayTracer(int spp = 1);
	Color GetPixelColor(int x, int y, Camera& camera, const Scene& scene);

private:
	int m_samplesPerPixel;
	float m_samplesPerPixelScale;

	Color GetBackgroundColor(const Ray& ray);
	Color TraceRay(Ray& primaryRay, const Scene& scene);

	const Interval k_rayLength = Interval(0, INFINITY);
};