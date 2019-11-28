// Fall 2019

#include "Player.hpp"

#include <iostream>
#include <sstream>

using namespace std;
using namespace glm;

static GLuint MAX_HP = 100; 

//---------------------------------------------------------------------------------------
Player::Player(SceneNode * node)
  : player_model(node),
    hp(MAX_HP),
    ammo(1),
    isShooting(false)
{

}

void Player::move(const glm::vec3 & amount) {
    // Disallow moving along the Y-axis
    player_model->translate(vec3(amount.x, 0.0f, amount.z));
}

// If ammo is available, decrement ammo and then return true, else false
bool Player::shoot() {
    if (ammo > 0) {
      isShooting = true;
      ammo = 0;
    }
    return isShooting;
}

void Player::setAmmo(GLuint ammo) {
  this->ammo = ammo;
}
