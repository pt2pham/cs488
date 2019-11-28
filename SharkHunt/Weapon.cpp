// Fall 2019

#include "Weapon.hpp"

#include <iostream>
#include <sstream>

using namespace std;
using namespace glm;

static const float VELOCITY = 0.05f;
static const float MAX_RANGE = 15.0f;

//---------------------------------------------------------------------------------------
Weapon::Weapon(SceneNode * node)
  : model(node),
    translation(vec3(0, 0, 0)),
    start_pos(vec3(0, 0, 0))
{

}

Weapon::~Weapon() {
    delete model;
}

void Weapon::setTrajectory(glm::vec3 pos, glm::vec3 direction) {
    // Set initial state for object model
    // TODO: Make sure the weapon is shot from a fixed position on the screen
    model->translate(vec3(pos.x + 0.2, pos.y - 0.4, pos.z - 1.5));

    // Set direction that the weapon should fire
    this->direction = direction;
}

void Weapon::animate() {
    vec3 trans = direction * VELOCITY;

    translation += trans;
    model->translate(trans);
}

bool Weapon::isExpired() {
    return abs(translation.z) > MAX_RANGE;
}

void Weapon::reset() {
    translation = vec3(0, 0, 0);
}
