#include "bar.h"

/**
 * @brief Construct a new Bar:: Bar object
 *
 * @param x position on the x axis
 * @param y posityion on the y axis
 * @param s intial load of the bar
 * @param mS maximum load of the bar
 * @param w width of the bar
 * @param h height of the bar
 */
Bar::Bar(float x, float y, int s, int mS, float w, float h) : Obj(x, y) {
    size = s;
    maxSize = mS;
    width = w;
    height = h;
}

Bar::~Bar() {}