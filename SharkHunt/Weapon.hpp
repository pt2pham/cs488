// Fall 2019

#pragma once

#include "Material.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "SceneNode.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <list>
#include <string>
#include <iostream>
#include <stack>
#include <queue>

class Weapon {
public:
    Weapon(SceneNode * node);
    ~Weapon();

    /*
    * Move the thing
    */
    void animate();

    /*
    * Check if the lifetime of the cannon should be over
    */
    bool isExpired();

    /*
    * Set translation back to (0,0,0)
    */
    void reset();

    void setTrajectory(glm::vec3 pos, glm::vec3 direction);

    friend std::ostream & operator << (std::ostream & os, const Weapon & node);

    SceneNode * model; 
private:
    /*
    * How much the cannon should be translated compared to its origin
    */
    glm::vec3 translation;

    /*
    * Duration that the cannonball lasts
    */
    float lifetime;
    glm::vec3 start_pos;
    glm::vec3 direction;
};
