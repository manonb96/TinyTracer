#pragma once
#include "../scene/scene.hpp"
#include "../scene/camera.hpp"
#include "ray.hpp"
#include "acceleration_structures/bvh.hpp"

class RayTracer {
public:
	RayTracer(int spp = 1);
	~RayTracer();
	void BuildBVH(const Scene& scene);
	Color GetPixelColor(int x, int y, Camera& camera, const Scene& scene);

private:
	int m_samplesPerPixel;
	float m_samplesPerPixelScale;

	BVHNode* m_pBVHTree = nullptr;

	Color GetBackgroundColor(const Ray& ray);
	Color TraceRay(Ray& primaryRay, const Scene& scene);

	Interval m_rayLength = Interval(0, INFINITY);
};