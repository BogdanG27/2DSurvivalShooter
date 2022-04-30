#pragma once

#include <vector>

#include "object.h"
#include "obstacle.h"

using namespace std;

class Projectile : public Obj {
   public:
    float width, height, angle, speed;
    float distance, maxDistance = 5;

    Projectile(float x, float y, float w, float h, float ang, float sp);
    ~Projectile();
    bool collides(vector<Obstacle*> obstacles, float distX, float distY);
    bool collides(float mapWidth, float mapHeight);
};