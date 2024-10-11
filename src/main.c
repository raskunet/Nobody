#include <math.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#define MIN_WIN_HEIGHT 1920
#define MIN_WIN_WIDTH 1080

#define MAX_BODIES 1 << 4
#define degToRad(angleInDegrees) ((angleInDegrees) * PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / PI)

#define arrSize(array) ((sizeof(array) / sizeof((array)[0])))

typedef struct CIRCLE {
    Vector2 pos;
    Vector2 originPos;
    Color color;
    float radius;
} Circle;

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(MIN_WIN_HEIGHT, MIN_WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);

    Color colorArr[] = {
        LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
        RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
        DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN,
        WHITE,     BLACK,  BLANK,    MAGENTA,    RAYWHITE};

    int x, y;
    int radius = 100;
    int angle = 0;
    int frameCounter = 0;

    Circle circleArr[MAX_BODIES];
    for (int i = 0; i < MAX_BODIES; i++) {
        circleArr[i].radius = GetRandomValue(50, 250);
        circleArr[i].color = colorArr[GetRandomValue(0, arrSize(colorArr))];
        circleArr[i].originPos.x =
            GetRandomValue(circleArr[i].radius * 2,
                           GetScreenHeight() - circleArr[i].radius * 2);
        circleArr[i].originPos.y =
            GetRandomValue(circleArr[i].radius * 2,
                           GetScreenWidth() - circleArr[i].radius * 2);
    }

    while (!WindowShouldClose()) {
        x = 40 + cos(degToRad(angle)) * (radius + 20);
        y = 40 + sin(degToRad(angle)) * (radius);

        for (int i = 0; i < MAX_BODIES; i++) {
            circleArr[i].pos.y = circleArr[i].originPos.x +
                                 sin(degToRad(angle)) * circleArr[i].radius;
            circleArr[i].pos.x = circleArr[i].originPos.y +
                                 cos(degToRad(angle)) * circleArr[i].radius;
        }

        // frameCounter++;
        //  if (frameCounter > 2) {
        //      frameCounter = 0;

        // }
        angle += 2.0f;

        BeginDrawing();
        ClearBackground(BLACK);
        // DrawText("N Body", GetScreenWidth() / 2, GetScreenHeight() / 2, 40,
        // WHITE);
        //  for (int i = 0; i < MAX_BODIES; i++) {
        //      DrawCircle(x + GetRandomValue(GetScreenWidth() / 2,
        //      GetScreenWidth() / 2 + 100),
        //                 y + GetRandomValue(GetScreenHeight() / 2,
        //                 GetScreenHeight() / 2 + 100), 5.0f,
        //                 colorArr[GetRandomValue(0, 25)]);
        //  }
        //  DrawCircle(x + GetScreenWidth() / 2, y + GetScreenHeight() / 2, 05,
        //  RED); DrawEllipseLines(40 + GetScreenWidth() / 2, 40 +
        //  GetScreenHeight() / 2, radius + 20, radius, RED);
        for (int i = 0; i < MAX_BODIES; i++) {
            DrawCircleV(circleArr[i].pos, 5, circleArr[i].color);
        }

        EndDrawing();
    }
    CloseWindow();
}