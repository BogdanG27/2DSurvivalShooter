#pragma once

#include "character.h"

/**
 * @brief Construct a new Character:: Character object
 *
 * @param x position on the x axis
 * @param y position on the y axis
 * @param rad radius of the circle
 * @param ang the angle of rotation
 */
Character::Character(float x, float y, float rad, float ang) : Obj(x, y) {
    radius = rad;
    angle = ang;
    speed = 3;
}

Character::~Character() {}

/**
 * @brief verifies the collision with a list of obstacles
 *
 * @param obstacles the list of all obstacles on the map
 * @param distX the distance moved on x axis
 * @param distY the distance moved on the y axis
 * @return the result of the collision
 */
bool Character::collides(vector<Obstacle*> obstacles, float distX, float distY) {
    for (int i = 0; i < obstacles.size(); i++)
        if (posX + distX - radius < obstacles[i]->posX + obstacles[i]->width &&
            posX + distX + radius > obstacles[i]->posX &&
            posY + distY - radius < obstacles[i]->posY + obstacles[i]->height &&
            radius + posY + distY > obstacles[i]->posY)
            return true;

    return false;
}