// Fall 2019

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "Hit.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual Hit intersect(const Ray & ray);
  Primitive * nonhier_prim;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  // virtual Hit intersect(const Ray & ray);
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  // virtual Hit intersect(const Ray & ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual Hit intersect(const Ray & ray);

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual Hit intersect(const Ray & ray);

private:
  glm::vec3 m_pos;
  double m_size;
};
