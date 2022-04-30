#include "obstacle.h"

/**
 * @brief Construct a new Obstacle:: Obstacle object
 *
 * @param x position on the x axis
 * @param y position on the y axis
 * @param w width of the obstacle
 * @param h height of the obstacle
 */
Obstacle::Obstacle(float x, float y, float w, float h) : Obj(x, y) {
    width = w;
    height = h;
}

Obstacle::~Obstacle() {
}