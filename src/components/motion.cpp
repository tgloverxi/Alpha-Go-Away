//
// Created by grumpy on 2021-03-08.
//

#include "motion.hpp"
void BoundingBox::scale(vec2 scale)
{
    mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
    for (vec2 v : vertices) {
        auto temp = S * vec3(v.x, v.y, 1.0);
        v = vec2(temp.x, temp.y);
    }
}

void BoundingBox::rotate(float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);
    mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
    for (vec2 v : vertices) {
        auto temp = R * vec3(v.x, v.y, 1.0);
        v = vec2(temp.x, temp.y);
    }
}

void BoundingBox::translate(vec2 offset) {
    mat3 T = { {1.f,      0.f,      0.f},
               {0.f,      1.f,      0.f},
               {offset.x, offset.y, 1.f} };
    for (vec2 v : vertices) {
        auto temp = T * vec3(v.x, v.y, 1.0);
        v = vec2(temp.x, temp.y);
    }
}