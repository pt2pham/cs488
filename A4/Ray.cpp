// Fall 2019

#include <iostream>

#include <glm/ext.hpp>

#include "Ray.hpp"

// ---- generate a blank Ray
Ray::Ray()
  : origin(glm::vec4(0, 0, 0, 1)), // point
    direction(glm::vec4(0, 0, 0, 0)) // vector
{
}

// --- Main constructor
Ray::Ray(glm::vec4 origin, glm::vec4 direction)
  : origin(origin), // point
    direction(direction - origin) // vector
{
}

Ray::~Ray() 
{
}

// --- Copy without doing math to direction
Ray Ray::rayCopy(glm::vec4 origin, glm::vec4 direction)
{
  return Ray(origin, origin + direction);
}
