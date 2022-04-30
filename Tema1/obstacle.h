#pragma once

#include "enemy.h"
#include "object.h"

class Obstacle : public Obj {
   public:
    float width, height;

    Obstacle(float x, float y, float w, float h);
    ~Obstacle();
};