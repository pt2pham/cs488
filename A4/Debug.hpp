// Helper class to throw Debugging functions into

#pragma once

#include <iostream>
#include <glm/glm.hpp>

class Debug {
public:
    static void print(glm::vec4 v) {
        std::cout << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]" << std::endl;
    }

    static void print(glm::vec3 v) {
        std::cout << "[" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
    }
};
