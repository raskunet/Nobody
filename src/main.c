#include <math.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#define MIN_WIN_HEIGHT GetMonitorHeight(0)
#define MIN_WIN_WIDTH GetMonitorWidth(0)

#define MAX_BODIES 3
#define degToRad(angleInDegrees) ((angleInDegrees) * PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / PI)

#define arrSize(array) ((sizeof(array) / sizeof((array)[0])))

typedef struct BODY {
    Vector2 pos;
    Vector2 originPos;
    Color color;
    float orbitRadius;
} Body;
;

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(MIN_WIN_HEIGHT, MIN_WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);

    Color colorArr[] = {
        LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
        RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
        DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN,
        WHITE,     BLACK,  BLANK,    MAGENTA,    RAYWHITE};

    int angle = 0;
    int frameCounter = 0;

    Body bodyArr[MAX_BODIES];
    for (int i = 0; i < MAX_BODIES; i++) {
        bodyArr[i].orbitRadius = GetRandomValue(50, 100);
        bodyArr[i].color = colorArr[GetRandomValue(0, arrSize(colorArr))];
        bodyArr[i].originPos.x =
            GetRandomValue(bodyArr[i].orbitRadius * 2,
                           GetScreenWidth() - bodyArr[i].orbitRadius * 2);
        bodyArr[i].originPos.y =
            GetRandomValue(bodyArr[i].orbitRadius * 2,
                           GetScreenHeight() - bodyArr[i].orbitRadius * 2);
    }

    while (!WindowShouldClose()) {
        for (int i = 0; i < MAX_BODIES; i++) {
            bodyArr[i].pos.y = bodyArr[i].originPos.y-2.5 +
                               sin(degToRad(angle)) * bodyArr[i].orbitRadius;
            bodyArr[i].pos.x = bodyArr[i].originPos.x -2.5 +
                               cos(degToRad(angle)) * bodyArr[i].orbitRadius;
        }

        angle = angle > 360 ? 0 : angle + 1.0f;

        BeginDrawing();
        if (IsKeyDown(KEY_SPACE)) {
            ClearBackground(WHITE);
        } else
            ClearBackground(BLACK);

        for (int i = 0; i < MAX_BODIES; i++) {
            DrawCircleV(bodyArr[i].pos, 5, bodyArr[i].color);
        }

        EndDrawing();
    }
    CloseWindow();
}