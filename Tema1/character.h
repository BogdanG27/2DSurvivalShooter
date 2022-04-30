#pragma once

#include <vector>

#include "object.h"
#include "obstacle.h"

using namespace std;

class Character : public Obj {
   public:
    float radius;
    float angle;
    float speed;

    Character(float x, float y, float rad, float ang);
    ~Character();
    bool collides(vector<Obstacle*> obstacles, float distX, float distY);
};