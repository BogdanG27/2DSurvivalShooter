#pragma once

#include "object.h"

// Bar object -> used for the score bar and the health bar
// extends the Obj class
class Bar : public Obj {
   public:
    int size, maxSize;
    float width, height;

    Bar(float x, float y, int s, int mS, float w, float h);
    ~Bar();
};