#pragma once

#include "object.h"

class Enemy : public Obj {
   public:
    float speed;
    float angle;
    float size = 1.0f;

    Enemy(float x, float y, float sp, float ang);
    ~Enemy();
};