#include "projectile.h"

/**
 * @brief Construct a new Projectile:: Projectile object
 *
 * @param x position x
 * @param y position y
 * @param w width
 * @param h height
 * @param ang angle
 * @param sp speed
 */
Projectile::Projectile(float x, float y, float w, float h, float ang, float sp) : Obj(x, y) {
    width = w;
    height = h;
    angle = ang;
    speed = sp;
    distance = 0;
}

Projectile::~Projectile() {}

/**
 * @brief verifies the collision with a list of obstacles
 *
 * @param obstacles list of all map obstacles
 * @param distX distance moved on x axis
 * @param distY distance moved on y axis
 * @return the result of the collision
 */
bool Projectile::collides(vector<Obstacle*> obstacles, float distX, float distY) {
    for (int i = 0; i < obstacles.size(); i++) {
        if (posX + distX - width / 2 < obstacles[i]->posX + obstacles[i]->width &&
            posX + distX + width / 2 > obstacles[i]->posX &&
            posY + distY - height / 2 < obstacles[i]->posY + obstacles[i]->height &&
            height / 2 + posY + distY > obstacles[i]->posY)
            return true;
    }
    return false;
}

/**
 * @brief verifies the collision with the edges of the map
 *
 * @param mapWidth width of the map
 * @param mapHeight height of the map
 * @return the result of the collision
 */
bool Projectile::collides(float mapWidth, float mapHeight) {
    if (posX + (width * 1 / 2) >= mapWidth ||
        posX - (width * 1 / 2) <= 0 ||
        posY + (height * 1 / 2) >= mapHeight ||
        posY - (height * 1 / 2) <= 0)
        return true;

    return false;
}