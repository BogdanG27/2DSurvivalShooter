#include "objects.h"

#include <math.h>

#include <iostream>

using namespace std;

/**
 * @brief Creates a circle mesh
 *
 * @param name name of the object
 * @param center center of the circle
 * @param radius radius of the circle
 * @param color color of the circle
 * @param fill option for filling the circle
 * @return Mesh*
 */
Mesh* createCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill) {
    int precizion = 1000;

    float x, y;
    std::vector<VertexFormat> vertices;

    // create a lot of triangles
    for (float i = 0; i < 2 * AI_MATH_PI; i += 2 * (float)AI_MATH_PI / precizion) {
        x = (float)center[0] + radius * cos(i);
        y = (float)center[1] + radius * sin(i);
        vertices.push_back(VertexFormat(center + glm::vec3(x, y, 0), color));
    }

    std::vector<unsigned int> indices;

    Mesh* circle = new Mesh(name);
    for (int i = 0; i < precizion; i++) {
        indices.push_back(i);
    }
    circle->InitFromData(vertices, indices);

    // use the TRIANGLE FAN for the filled circle
    // and GL LINE LOOP for only the outline
    if (fill == false) {
        circle->SetDrawMode(GL_LINE_LOOP);
    } else {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
    }

    return circle;
}

/**
 * @brief Create a rectanngle mesh
 *
 * @param name name of the reclangle
 * @param leftBottomCorner starting spot
 * @param width width
 * @param height height
 * @param color color
 * @param fill fill option
 * @param mode option for drawing relative to the bottom left corner or the center of the rectangle
 * @return Mesh*
 */
Mesh* createRect(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill,
    int mode) {
    glm::vec3 spot;

    // if it's corner mode draw relative to the bottom left corner
    // else substract half the width and the height from the corner to draw the rectangle relative to it's center
    if (mode == MODE_CORNER)
        spot = leftBottomCorner;
    else {
        spot[0] = leftBottomCorner[0] - width / 2;
        spot[1] = leftBottomCorner[1] - height / 2;
        spot[2] = 0;
    }

    std::vector<VertexFormat> vertices =
        {
            VertexFormat(spot, color),
            VertexFormat(spot + glm::vec3(width, 0, 0), color),
            VertexFormat(spot + glm::vec3(width, height, 0), color),
            VertexFormat(spot + glm::vec3(0, height, 0), color)};

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = {0, 1, 2, 3};
    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}