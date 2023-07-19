#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "texture.h"
using std::shared_ptr;

class material;

struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    shared_ptr<texture>mat_ptr;
    float t;
    float u;
    float v;
    bool front_face;

    inline void set_face_normal(const ray& r, const glm::vec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;
    virtual glm::vec3 color(glm::vec2 uv) const = 0;
};

#endif
