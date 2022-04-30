#include "Tema1.h"

#include <math.h>

#include <iostream>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1() {
}

Tema1::~Tema1() {
}

void Tema1::Init() {
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    view = new View(16, 9);

    logicSpace.x = (float)view->width / 2;    // logic x
    logicSpace.y = (float)view->height / 2;   // logic y
    logicSpace.width = (float)view->width;    // logic width
    logicSpace.height = (float)view->height;  // logic height

    character = new Character((float)mapWidth / 2, (float)mapHeight / 2, 0.5f, 0);
    scoreBar = new Bar(0, 0, 0, 40, 3.0f, 0.5f);
    healthBar = new Bar(0, 0, 10, 10, 3.0f, 0.5f);

    CreateObstacles();

    glm::vec3 corner = glm::vec3(0, 0, 0);
    length = 1;

    Mesh* backgr = createRect("backgr", corner, (float)mapWidth, (float)mapHeight, glm::vec3(0.8, 0.8, 0.8), true, MODE_CORNER);
    Mesh* innerCircle = createCircle("innerCircle", corner, (float)(character->radius), glm::vec3(1, 1, 0), true);
    Mesh* outerCircle = createCircle("outerCircle", corner, character->radius, glm::vec3(0, 0, 0), false);
    Mesh* projectile = createRect("Projectile", corner, 0.5f, 0.25f, glm::vec3(0, 0, 0), true, MODE_CENTER);
    Mesh* enemy = createRect("enemy", corner, 1.0f, 1.0f, glm::vec3(1, 0, 0), true, MODE_CENTER);
    Mesh* enemy2 = createRect("enemy2", corner, 1.0f, 1.0f, glm::vec3(0, 0, 0), false, MODE_CENTER);
    Mesh* scoreBar = createRect("scoreBar", corner, 3.0f, 0.5f, glm::vec3(0, 0, 1), false, MODE_CORNER);
    Mesh* healthBar = createRect("healthBar", corner, 3.0f, 0.5f, glm::vec3(1, 0, 0), false, MODE_CORNER);
    Mesh* scoreBarComponent = createRect("scoreBarComponent", corner, 1.0f, 1.0f, glm::vec3(0, 0, 1), true, MODE_CORNER);
    Mesh* healthBarComponent = createRect("healthBarComponent", corner, 1.0f, 1.0f, glm::vec3(1, 0, 0), true, MODE_CORNER);
    Mesh* obstacle = createRect("obstacle", corner, 1.0f, 1.0f, glm::vec3(0, 1, 0), true, MODE_CORNER);
    Mesh* obstacleFrame = createRect("obstacleFrame", corner, 1.0f, 1.0f, glm::vec3(0, 0, 0), false, MODE_CORNER);

    AddMeshToList(backgr);
    AddMeshToList(innerCircle);
    AddMeshToList(outerCircle);
    AddMeshToList(projectile);
    AddMeshToList(enemy);
    AddMeshToList(enemy2);
    AddMeshToList(scoreBar);
    AddMeshToList(healthBar);
    AddMeshToList(scoreBarComponent);
    AddMeshToList(healthBarComponent);
    AddMeshToList(obstacle);
    AddMeshToList(obstacleFrame);
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace) {
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}

// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace) {
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear) {
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

void Tema1::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief updates that happen every frame
 *
 * @param deltaTimeSeconds frame time in seconds for the previous frame
 */
void Tema1::Update(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw - the left half of the window
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    // Compute the 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    UpdateEnemies(deltaTimeSeconds);
    UpdateProjectiles(deltaTimeSeconds);
    DrawScene(visMatrix);
}

/**
 * @brief creates a list of Obstacle objects to place on the map
 *
 */
void Tema1::CreateObstacles() {
    obstacles.push_back(new Obstacle(3.0f, 3.0f, 7.0f, 1.0f));
    obstacles.push_back(new Obstacle(8.0f, 2.0f, 1.0f, 1.0f));
    obstacles.push_back(new Obstacle(5.0f, 7.0f, 3.0f, 3.0f));
    obstacles.push_back(new Obstacle(3.0f, 12.0f, 1.0f, 3.0f));
    obstacles.push_back(new Obstacle(3.0f, 15.0f, 3.0f, 1.0f));
    obstacles.push_back(new Obstacle(5.0f, 14.0f, 1.0f, 1.0f));
    obstacles.push_back(new Obstacle(10.0f, 6.0f, 1.0f, 1.0f));
    obstacles.push_back(new Obstacle(9.0f, 13.0f, 8.0f, 2.0f));
    obstacles.push_back(new Obstacle(14.0f, 2.0f, 1.0f, 4.0f));
    obstacles.push_back(new Obstacle(15.0f, 3.0f, 3.0f, 2.0f));
    obstacles.push_back(new Obstacle(22.0f, 6.0f, 1.0f, 4.0f));
    obstacles.push_back(new Obstacle(23.0f, 6.0f, 3.0f, 1.0f));
    obstacles.push_back(new Obstacle(23.0f, 9.0f, 3.0f, 1.0f));
    obstacles.push_back(new Obstacle(22.0f, 13.0f, 2.0f, 5.0f));
    obstacles.push_back(new Obstacle(24.0f, 13.0f, 2.0f, 2.0f));
    obstacles.push_back(new Obstacle(29.0f, 11.0f, 1.0f, 3.0f));
    obstacles.push_back(new Obstacle(27.0f, 3.0f, 4.0f, 1.0f));
    obstacles.push_back(new Obstacle(28.0f, 4.0f, 1.0f, 3.0f));
    obstacles.push_back(new Obstacle(29.0f, 2.0f, 1.0f, 1.0f));
}

/**
 * @brief draws the obstacles created with @createObstacles function
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawObstacles(glm::mat3 visMatrix) {
    for (int i = 0; i < obstacles.size(); i++) {
        modelMatrix = visMatrix * transform2D::Translate(obstacles[i]->posX, obstacles[i]->posY) * transform2D::Scale(obstacles[i]->width, obstacles[i]->height);
        RenderMesh2D(meshes["obstacleFrame"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    }
}

/**
 * @brief updates the position of the enemies
 *
 * @param deltaTimeSeconds frame time in seconds for the previous frame
 */
void Tema1::UpdateEnemies(float deltaTimeSeconds) {
    // spawn the enemies acording to spawn time (decreases with levels)
    if (timeElapsed > spawnTime) {
        timeElapsed = 0;

        // generating random position for the enemy to spawm
        float X = character->posX;
        float Y = character->posY;
        while (sqrt(pow(X - character->posX, 2) + pow(Y - character->posY, 2)) < 15 * character->radius) {
            float LO = 0.5f;
            float HI = (float)mapWidth - 0.5f;
            X = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));

            float HI2 = (float)mapHeight - 0.5f;
            Y = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI2 - LO)));
        }

        // also random speed
        float sp = 2.0f + speedIncrease + rand() / (RAND_MAX / (3));

        enemies.push_back(new Enemy(X, Y, sp, 0));

    } else {
        timeElapsed += deltaTimeSeconds;
    }

    // check bulets collision
    vector<int> indicesP;
    vector<int> indicesE;
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < projectiles.size(); j++) {
            if ((projectiles[j]->posX + projectiles[j]->width / 2 >= enemies[i]->posX - enemies[i]->size / 2) &&
                (projectiles[j]->posX - projectiles[j]->width / 2 <= enemies[i]->posX + enemies[i]->size / 2) &&
                (projectiles[j]->posY + projectiles[j]->height / 2 >= enemies[i]->posY - enemies[i]->size / 2) &&
                (projectiles[j]->posY - projectiles[j]->height / 2 <= enemies[i]->posY + enemies[i]->size / 2)) {
                indicesP.push_back(j);
                indicesE.push_back(i);

                // if enemies collide with a bullet the score increases
                if (scoreBar->size < scoreBar->maxSize)
                    scoreBar->size++;

                // different things happend when score reaches a certain level
                if (scoreBar->size == LEVEL2_SCORE) {
                    spawnTime = 1.0f;
                    cout << "LEVEL 2 REACHED -> ENEMIES SPAWN FASTER\n";
                }

                if (scoreBar->size == LEVEL3_SCORE) {
                    speedIncrease = 2.0f;
                    cout << "LEVEL 3 REACHED -> ENEMIES ARE FASTER\n";
                }

                if (scoreBar->size == LEVEL4_SCORE) {
                    spawnTime = 0.5f;
                    speedIncrease = 3.0f;
                    cout << "LEVEL 4 REACHED -> ENEMIES SPAWN EVEN FASTER AND HAVE EVEN HIGHER SPEEDS\n";
                }
            }
        }
    }
    int j = 0;
    // removing the objects that collided (bullet and enemy)
    for (int ind : indicesE) {
        enemies.erase(enemies.begin() + (ind - j));
        j++;
    }
    j = 0;
    for (int ind : indicesP) {
        projectiles.erase(projectiles.begin() + (ind - j));
        j++;
    }

    vector<int> indicesC;
    // check character collision
    for (int i = 0; i < enemies.size(); i++) {
        if ((character->posX + character->radius >= enemies[i]->posX - enemies[i]->size / 2) &&
            (character->posX - character->radius <= enemies[i]->posX + enemies[i]->size / 2) &&
            (character->posY + character->radius >= enemies[i]->posY - enemies[i]->size / 2) &&
            (character->posY - character->radius <= enemies[i]->posY + enemies[i]->size / 2)) {
            indicesC.push_back(i);
            // if a enemy hits the player health bar decreases
            if (healthBar->size > 0)
                healthBar->size--;
        }
    }
    j = 0;
    // erase that enemy
    for (int ind : indicesC) {
        enemies.erase(enemies.begin() + (ind - j));
        j++;
    }

    // update the position of the enemy based on the player position
    for (int i = 0; i < enemies.size(); i++) {
        enemies[i]->angle = atan2(enemies[i]->posY - character->posY, enemies[i]->posX - character->posX);
        enemies[i]->posX -= deltaTimeSeconds * enemies[i]->speed * cos(enemies[i]->angle);
        enemies[i]->posY -= deltaTimeSeconds * enemies[i]->speed * sin(enemies[i]->angle);
    }

    // if health reaches 0 or score reaches maxScore the game is reseted
    if (healthBar->size <= 0 || scoreBar->size >= scoreBar->maxSize) {
        string result = (healthBar->size <= 0) ? "You lost! Game is reseted!\n" : "You won! Game is reseted!\n";
        scoreBar->size = 0;
        healthBar->size = healthBar->maxSize;
        spawnTime = 2.0f;
        speedIncrease = 0.0f;
        enemies.clear();
        projectiles.clear();
        cout << result;
    }
}

void Tema1::FrameEnd() {
}

/**
 * @brief draws the health bar and the score bar
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawBars(glm::mat3 visMatrix) {
    for (int i = 0; i < scoreBar->size; i++) {
        modelMatrix = visMatrix * transform2D::Translate(character->posX - logicSpace.width / 2 + 1, character->posY + logicSpace.height / 2 - 0.5f) * transform2D::Translate(i * (float)scoreBar->width / scoreBar->maxSize, 0) * transform2D::Scale((float)scoreBar->width / scoreBar->maxSize, scoreBar->height);
        RenderMesh2D(meshes["scoreBarComponent"], shaders["VertexColor"], modelMatrix);
    }
    modelMatrix = visMatrix * transform2D::Translate(character->posX - logicSpace.width / 2 + 1, character->posY + logicSpace.height / 2 - 0.5f);
    RenderMesh2D(meshes["scoreBar"], shaders["VertexColor"], modelMatrix);

    for (int i = 0; i < healthBar->size; i++) {
        modelMatrix = visMatrix * transform2D::Translate(character->posX + logicSpace.width / 2 - healthBar->width - 1, character->posY + logicSpace.height / 2 - 0.5f) * transform2D::Translate(i * (float)healthBar->width / healthBar->maxSize, 0) * transform2D::Scale((float)healthBar->width / healthBar->maxSize, healthBar->height);
        RenderMesh2D(meshes["healthBarComponent"], shaders["VertexColor"], modelMatrix);
    }
    modelMatrix = visMatrix * transform2D::Translate(character->posX + logicSpace.width / 2 - healthBar->width - 1, character->posY + logicSpace.height / 2 - 0.5f);
    RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix);
}

/**
 * @brief draws all the components of the character
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawCharacter(glm::mat3 visMatrix) {
    float sc = 0.2f;
    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY) * transform2D::Translate(character->radius * cos(character->angle - 0.5f), character->radius * sin(character->angle - 0.5f)) * transform2D::Scale(sc, sc);
    RenderMesh2D(meshes["outerCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY) * transform2D::Translate(character->radius * cos(character->angle - 0.5f), character->radius * sin(character->angle - 0.5f)) * transform2D::Scale(sc, sc);
    RenderMesh2D(meshes["innerCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY) * transform2D::Translate(character->radius * cos(character->angle + 0.5f), character->radius * sin(character->angle + 0.5f)) * transform2D::Scale(sc, sc);
    RenderMesh2D(meshes["outerCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY) * transform2D::Translate(character->radius * cos(character->angle + 0.5f), character->radius * sin(character->angle + 0.5f)) * transform2D::Scale(sc, sc);
    RenderMesh2D(meshes["innerCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY);
    RenderMesh2D(meshes["outerCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(character->posX, character->posY);
    RenderMesh2D(meshes["innerCircle"], shaders["VertexColor"], modelMatrix);
}

/**
 * @brief draws all the components of all the enemies
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawEnemies(glm::mat3 visMatrix) {
    float sc = 0.2f;

    for (int i = 0; i < enemies.size(); i++) {
        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Translate(-cos(enemies[i]->angle + 0.5f) * enemies[i]->size / 2, -sin(enemies[i]->angle + 0.5f) * enemies[i]->size / 2) * transform2D::Rotate(enemies[i]->angle) * transform2D::Scale(0.2f, 0.2f);
        RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Translate(-cos(enemies[i]->angle + 0.5f) * enemies[i]->size / 2, -sin(enemies[i]->angle + 0.5f) * enemies[i]->size / 2) * transform2D::Rotate(enemies[i]->angle) * transform2D::Scale(0.2f, 0.2f);
        RenderMesh2D(meshes["enemy2"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Translate(-cos(enemies[i]->angle - 0.5f) * enemies[i]->size / 2, -sin(enemies[i]->angle - 0.5f) * enemies[i]->size / 2) * transform2D::Rotate(enemies[i]->angle) * transform2D::Scale(0.2f, 0.2f);
        RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Translate(-cos(enemies[i]->angle - 0.5f) * enemies[i]->size / 2, -sin(enemies[i]->angle - 0.5f) * enemies[i]->size / 2) * transform2D::Rotate(enemies[i]->angle) * transform2D::Scale(0.2f, 0.2f);
        RenderMesh2D(meshes["enemy2"], shaders["VertexColor"], modelMatrix);

        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Rotate(enemies[i]->angle);
        RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);
        modelMatrix = visMatrix * transform2D::Translate(enemies[i]->posX, enemies[i]->posY) * transform2D::Rotate(enemies[i]->angle);
        RenderMesh2D(meshes["enemy2"], shaders["VertexColor"], modelMatrix);
    }
}

/**
 * @brief updates the position of th projectiles
 *
 * @param deltaTimeSeconds frame time in seconds for the previous frame
 */
void Tema1::UpdateProjectiles(float deltaTimeSeconds) {
    if (projectiles.size() != 0) {
        vector<int> indices;
        for (int i = 0; i < projectiles.size(); i++) {
            // check map edge colisions
            if (projectiles[i]->collides(obstacles,
                                         cos(projectiles[i]->angle) * projectiles[i]->speed * deltaTimeSeconds,
                                         sin(projectiles[i]->angle) * projectiles[i]->speed * deltaTimeSeconds) ||
                projectiles[i]->collides((float)mapWidth, (float)mapHeight) ||
                projectiles[i]->distance >= projectiles[i]->maxDistance) {
                indices.push_back(i);
            } else {
                // no collisions -> move the projectiles
                float offX = cos(projectiles[i]->angle) * projectiles[i]->speed * deltaTimeSeconds;
                float offY = sin(projectiles[i]->angle) * projectiles[i]->speed * deltaTimeSeconds;
                projectiles[i]->posX += offX;
                projectiles[i]->posY += offY;
                projectiles[i]->distance += sqrt(offX * offX + offY * offY);
            }
        }

        int j = 0;
        for (int el : indices) {
            projectiles.erase(projectiles.begin() + el - j);
            j++;
        }
    }
}

/**
 * @brief draws the projectiles
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawProjectiles(glm::mat3 visMatrix) {
    if (projectiles.size() != 0) {
        for (int i = 0; i < projectiles.size(); i++) {
            modelMatrix = visMatrix * transform2D::Translate(projectiles[i]->posX, projectiles[i]->posY) * transform2D::Rotate(projectiles[i]->angle);
            RenderMesh2D(meshes["Projectile"], shaders["VertexColor"], modelMatrix);
        }
    }
}

/**
 * @brief draws all the elements
 *
 * @param visMatrix visualization matrix
 */
void Tema1::DrawScene(glm::mat3 visMatrix) {
    modelMatrix = visMatrix * transform2D::Translate(0, 0);

    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    DrawBars(visMatrix);
    DrawCharacter(visMatrix);
    DrawProjectiles(visMatrix);
    DrawEnemies(visMatrix);
    DrawObstacles(visMatrix);

    modelMatrix = visMatrix * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["backgr"], shaders["VertexColor"], modelMatrix);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods) {
    // movement of the character and the logic space
    if (window->KeyHold(GLFW_KEY_S)) {
        if ((character->posY - character->radius) > 0 && !character->collides(obstacles, 0, -character->speed * deltaTime)) {
            logicSpace.y -= character->speed * deltaTime;
            character->posY -= character->speed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if ((character->posX + character->radius) < mapWidth && !character->collides(obstacles, character->speed * deltaTime, 0)) {
            logicSpace.x += character->speed * deltaTime;
            character->posX += character->speed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        if ((character->posY + character->radius) < mapHeight && !character->collides(obstacles, 0, character->speed * deltaTime)) {
            logicSpace.y += character->speed * deltaTime;
            character->posY += character->speed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        if ((character->posX - character->radius) > 0 && !character->collides(obstacles, -character->speed * deltaTime, 0)) {
            logicSpace.x -= character->speed * deltaTime;
            character->posX -= character->speed * deltaTime;
        }
    }
}

void Tema1::OnKeyPress(int key, int mods) {
    // zoom in and out
    if (GLFW_KEY_Z == key) {
        logicSpace.width += 1;
        logicSpace.height += 1;
        logicSpace.x = character->posX - (float)logicSpace.width / 2;  // logic x
        logicSpace.y = character->posY - (float)logicSpace.height / 2;
    } else if (GLFW_KEY_X == key) {
        logicSpace.width -= 1;
        logicSpace.height -= 1;
        logicSpace.x = character->posX - (float)logicSpace.width / 2;  // logic x
        logicSpace.y = character->posY - (float)logicSpace.height / 2;
    }
}

void Tema1::OnKeyRelease(int key, int mods) {
    // Add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    // Add mouse move event
    float d1, d2;

    auto res = window->GetResolution();
    res[0] /= 2;
    res[1] /= 2;

    d1 = (float)sqrt(pow((res[0] - mouseX), 2) + pow((res[1] - mouseY), 2));

    // cadranul 1
    if (mouseX >= res[0] && mouseY <= res[1]) {
        d2 = (float)sqrt(pow((res[0] - mouseX), 2));
        character->angle = acos(d2 / d1);
    }
    // cadranul 2
    else if (mouseX <= res[0] && mouseY <= res[1]) {
        d2 = (float)sqrt(pow((res[1] - mouseY), 2));
        character->angle = acos(d2 / d1) + AI_MATH_HALF_PI_F;
    } else if (mouseX <= res[0] && mouseY >= res[1]) {
        d2 = (float)sqrt(pow((res[0] - mouseX), 2));
        character->angle = acos(d2 / d1) + AI_MATH_PI_F;
    } else {
        d2 = (float)sqrt(pow((res[1] - mouseY), 2));
        character->angle = acos(d2 / d1) + AI_MATH_PI_F + AI_MATH_HALF_PI_F;
    }
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    // when click is presed, player shoots a bullet

    if (button == 1) {
        if (projectiles.size() < MAX_BULLETS) {
            projectiles.push_back(new Projectile(
                character->posX + character->radius * cos(character->angle),
                character->posY + character->radius * sin(character->angle),
                0.5f,
                0.25f,
                character->angle,
                character->speed));
        }
    }
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}
