#pragma once

// Generic object -> only has a position, a x and a y
class Obj {
   public:
    float posX, posY;

    Obj(float x, float y);
    ~Obj();
};