#include "enemy.h"

/**
 * @brief Construct a new Enemy:: Enemy object
 *
 * @param x position x
 * @param y position y
 * @param sp speed of the enemy
 * @param ang angle of rotation
 */
Enemy::Enemy(float x, float y, float sp, float ang) : Obj(x, y) {
    speed = sp;
    angle = ang;
}

Enemy::~Enemy() {}