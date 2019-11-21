// Fall 2019

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

class Ray {
public:
    Ray();
    Ray(glm::vec4 origin, glm::vec4 direction);
    static Ray rayCopy(glm::vec4 origin, glm::vec4 direction);

    ~Ray();

    glm::vec4 origin;
    glm::vec4 direction;
};
