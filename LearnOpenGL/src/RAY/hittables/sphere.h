#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "../hittable.h"

class sphere : public hittable {
public:
    sphere() {}
    sphere(glm::vec3 cen, float r, shared_ptr<texture> m)
        : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;
    virtual glm::vec3 color(glm::vec2 uv) const override
    {
        return mat_ptr->value(uv.x, uv.y);
    }
public:
    glm::vec3 center;
    float radius;
    shared_ptr<texture>mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    glm::vec3 oc = r.origin() - center;
    auto a = glm::length(r.direction());
    a = a * a;
    auto half_b = dot(oc, r.direction());
    auto c = glm::length(oc) * glm::length(oc) - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    glm::vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    //get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
    output_box = aabb(
        center - glm::vec3(radius, radius, radius),
        center + glm::vec3(radius, radius, radius));
    return true;
}

#endif