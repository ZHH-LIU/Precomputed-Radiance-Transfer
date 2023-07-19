#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

inline float clamp(float x, float min, float max)
{
    if (x > max)
        return max;
    if (x < min)
        return min;
    return x;
}

class texture {
public:
    virtual glm::vec3 value(float u, float v) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(glm::vec3 c) : color_value(c) {}

    solid_color(float red, float green, float blue)
        : solid_color(glm::vec3(red, green, blue)) {}

    virtual glm::vec3 value(float u, float v) const override {
        return color_value;
    }

private:
    glm::vec3 color_value;
};

class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture();

    image_texture(const char* filename);

    ~image_texture();

    virtual glm::vec3 value(float u, float v) const override;

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};

#endif