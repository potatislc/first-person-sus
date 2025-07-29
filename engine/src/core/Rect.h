#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct Rect {
    union {
        struct {
            glm::vec2 origin;
            glm::vec2 size;
        };

        struct {
            float x, y, width, height;
        };

        glm::vec4 vec4;
    };
};
