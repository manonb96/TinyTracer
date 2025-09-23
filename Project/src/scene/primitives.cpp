#include "primitives.hpp"

Primitive::Primitive(float3 position, Color albedo) : m_center(position), m_albedo(albedo) {}

Color Primitive::GetAlbedo() const {
    return m_albedo;
}

void Primitive::SetID(uint id) {
    m_id = id;
}

uint Primitive::GetID() const {
    return m_id;
}

Sphere::Sphere(float3 center, float radius, Color albedo) : Primitive(center, albedo), m_radius(radius) {
    float3 rvec = float3(m_radius, m_radius, m_radius);
    m_bbox = AABB(m_center - rvec, m_center + rvec);
}

// We assume here that ray.direction is normalized
bool Sphere::Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const {
    float3 originToCenter = m_center - ray.origin;
    float tClosestApproach = dot(originToCenter, ray.direction);
    float distanceToSurfaceSquared = lengthSquared(originToCenter) - m_radius * m_radius;

    float discriminant = tClosestApproach * tClosestApproach - distanceToSurfaceSquared;
    if (discriminant < 0)
    {
        return false;
    }

    float tHalfChord = sqrt(discriminant);

    float root = tClosestApproach - tHalfChord;
    if (root <= ray_t.min || ray_t.max <= root) {
        root = tClosestApproach + tHalfChord;
        if (root <= ray_t.min || ray_t.max <= root) {
            return false;
        }
    }

    intersectionPoint.t = root;
    intersectionPoint.point = ray.GetPoint(root);
    intersectionPoint.normal = (intersectionPoint.point - m_center) * (1.0f / m_radius);
    intersectionPoint.objectID = m_id;

    return true;
}

//bool Sphere::Hit(const Ray& ray, Interval& ray_t, IntersectionPoint& intersectionPoint) const {
//    float3 oc = center - ray.origin;
//    float tca = dot(oc, ray.direction);
//    // if (tca < 0) return false;
//
//    float r2 = radius * radius;
//    float d2 = dot(oc, oc) - tca * tca;
//    if (d2 > r2) {
//        return false;
//    }
//
//    float thc = sqrt(r2 - d2);
//    float t0 = tca - thc;
//    float t1 = tca + thc;
//
//    if (t0 > t1) {
//        std::swap(t0, t1);
//    }
//    
//    if (t0 < 0) {
//        t0 = t1;
//    }
//
//    ray.t = t0;
//
//    if (t0 < 0) {
//        return false;
//    }
//    
//    return true;
//}