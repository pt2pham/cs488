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

class Player {
public:
    Player(SceneNode * node);
    ~Player();

    /*
    * Based on what gets passed, move the character along the plane;
    * should basically just pass on to the translate method in SceneNode
    * to translate the Player in that way.
    * 
    * There should probably be a bound so that the player cannot move along the Y-axis
    * just in case - although the command is never passed from main, it doesn't hurt?
    */
    void move(const glm::vec3 & amount);
    friend std::ostream & operator << (std::ostream & os, const Player & node);

private:
    /*
    * On every collision with the Enemy, reduce HP by 1;
    */
    GLuint hp;
    /*
    * On every attack, reduce ammo by 1
    * Ideally there shouldn't be multiple balls shot because bump mapping the water 
    * will be difficult with ripples touching each other, so maybe ammo should 
    * always be 1, and this value should just be a cooldown for when you can 
    * shoot again - let the cooldown equal how long the ripple effect lasts 
    * in the water?
    */
    GLuint ammo;

    /*
    * Will probably load the lua script required to generate the character here 
    */
    SceneNode * player_model; 

	
};
