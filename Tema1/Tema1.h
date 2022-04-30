#pragma once

#include <vector>

#include "bar.h"
#include "character.h"
#include "components/simple_scene.h"
#include "enemy.h"
#include "objects.h"
#include "obstacle.h"
#include "projectile.h"
#include "transform2D.h"
#include "view.h"

#define MAX_BULLETS 5
#define LEVEL2_SCORE 10
#define LEVEL3_SCORE 20
#define LEVEL4_SCORE 30

using namespace std;

namespace m1 {
class Tema1 : public gfxc::SimpleScene {
   public:
    struct ViewportSpace {
        ViewportSpace() : x(0), y(0), width(1), height(1) {}
        ViewportSpace(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {}
        int x;
        int y;
        int width;
        int height;
    };

    struct LogicSpace {
        LogicSpace() : x(0), y(0), width(1), height(1) {}
        LogicSpace(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) {}
        float x;
        float y;
        float width;
        float height;
    };

   public:
    Tema1();
    ~Tema1();

    void Init() override;

   private:
    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void CreateObstacles();
    void DrawScene(glm::mat3 visMatrix);
    void DrawCharacter(glm::mat3 visMatrix);
    void DrawProjectiles(glm::mat3 visMatrix);
    void DrawEnemies(glm::mat3 visMatrix);
    void DrawBars(glm::mat3 visMatrix);
    void DrawObstacles(glm::mat3 visMatrix);
    void UpdateProjectiles(float deltaTimeSeconds);
    void UpdateCharacter();
    void UpdateEnemies(float deltaTimeSeconds);
    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

    // Sets the logic space and view space
    // logicSpace: { x, y, width, height }
    // viewSpace: { x, y, width, height }
    glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
    glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

    void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

   protected:
    float length, timeElapsed = 0, spawnTime = 2.0f, speedIncrease = 0.0f;
    int logicSpaceSize = 8;
    int mapWidth = 32, mapHeight = 18;
    View* view;
    Character* character;
    vector<Projectile*> projectiles;
    vector<Enemy*> enemies;
    vector<Obstacle*> obstacles;
    Bar* scoreBar;
    Bar* healthBar;
    ViewportSpace viewSpace;
    LogicSpace logicSpace;
    glm::mat3 modelMatrix, visMatrix;
};
}  // namespace m1
