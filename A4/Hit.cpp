// Fall 2019

#include <iostream>

#include "Hit.hpp"

Hit::Hit()
  : t(0), 
    hit(false)
{
}

Hit::Hit(double t, bool hit, glm::vec4 normal)
  : t(t),
    hit(hit),
    normal(normal)
{
}

Hit::~Hit() 
{
}
