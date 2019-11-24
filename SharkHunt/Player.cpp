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
    ammo(1)
{

}

void Player::move(const glm::vec3 & amount) {
    // Disallow moving along the Y-axis
    player_model->translate(vec3(amount.x, 0.0f, amount.z));
}
