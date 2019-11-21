// Fall 2019

#include "Primitive.hpp"
#include "polyroots.hpp"
#include "Debug.hpp"

using namespace glm;
using namespace std;

Primitive::~Primitive()
{
}

Hit Primitive::intersect(const Ray & ray) {
    // Should be implemented in the base classes
	return nonhier_prim->intersect(ray);
}

Sphere::~Sphere()
{
}

Sphere::Sphere() 
{
    nonhier_prim = new NonhierSphere({0, 0, 0}, 1.0);    
}

Cube::~Cube()
{
}

Cube::Cube()
{
    nonhier_prim = new NonhierBox({0, 0, 0}, 1.0);
}

NonhierSphere::~NonhierSphere()
{
}

Hit NonhierSphere::intersect(const Ray & ray) {
    vec4 origin = ray.origin;
    vec4 dir = ray.direction;
    vec4 ray_to_sphere = ray.origin - vec4(m_pos, 1);

    // Quadratic solve
    double A = glm::dot(dir, dir);
    double B = 2 * glm::dot(dir, ray_to_sphere);
    double C = glm::dot(ray_to_sphere, ray_to_sphere) - (m_radius * m_radius);
    double roots[2];
    size_t numOfRoots = quadraticRoots(A, B, C, roots);

    double t;
    glm::vec4 point, normal;
    switch (numOfRoots) {
        case 0:
            return Hit(); // Empty hit
        case 1:
            t = roots[0];
            // Calculate the Normal
            point = origin + dir * (float)t;
            normal = glm::normalize(point - vec4(m_pos, 1));
            return t > 0 ? Hit(t, true, normal) : Hit();
        case 2:
            t = std::min(roots[0], roots[1]);
            // Calculate the Normal
            point = origin + dir * (float)t;
            normal = glm::normalize(point - vec4(m_pos, 1));
            return t > 0 ? Hit(t, true, normal) : Hit();
        default:
            break;
    }
}

NonhierBox::~NonhierBox()
{
}

Hit NonhierBox::intersect(const Ray & r) {
    glm::vec3 bounds[2] = {m_pos, m_pos + glm::vec3(m_size, m_size, m_size)};

    float tmin = (bounds[0].x - r.origin.x) / r.direction.x; 
    float tmax = (bounds[1].x - r.origin.x) / r.direction.x; 
 
    if (tmin > tmax) swap(tmin, tmax); 
 
    float tymin = (bounds[0].y - r.origin.y) / r.direction.y; 
    float tymax = (bounds[1].y - r.origin.y) / r.direction.y; 
 
    if (tymin > tymax) swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return Hit(); 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (bounds[0].z - r.origin.z) / r.direction.z; 
    float tzmax = (bounds[1].z - r.origin.z) / r.direction.z; 
 
    if (tzmin > tzmax) swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return Hit(); 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 

    if (tmin < INFINITY && tmax > 0) {
        // CALCULATE NORMAL
        vec4 point = r.origin + r.direction * tmin;
        vec4 normal;
        double epsilon = 0.001;

        if (fabs(point.x - (m_pos.x + m_size)) <= epsilon) {
            normal = vec4(1, 0, 0, 0);
        } else if (fabs(point.x - m_pos.x) <= epsilon) {
            normal = vec4(-1, 0, 0, 0);
        } else if (fabs(point.y - (m_pos.y + m_size)) <= epsilon) {
            normal = vec4(0, 1, 0, 0);
        } else if (fabs(point.y - m_pos.y) <= epsilon) {
            normal = vec4(0, -1, 0, 0);
        } else if (fabs(point.z - (m_pos.z + m_size)) <= epsilon) {
            normal = vec4(0, 0, 1, 0);
        } else if (fabs(point.z - m_pos.z) <= epsilon) {
            normal = vec4(0, 0, -1, 0);
        }

        return Hit(tmin, true, normalize(normal)); 
    }

    // If we're here then it missed
    return Hit();
}
