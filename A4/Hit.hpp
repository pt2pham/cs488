// Fall 2019

#pragma once

#include "PhongMaterial.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

class Hit {
public:
    Hit();
    Hit(double t, bool hit, glm::vec4 normal);

    ~Hit();

    glm::vec4 normal;
    bool hit;
    double t;
    PhongMaterial * mat;
};
