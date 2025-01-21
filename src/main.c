#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "body.h"
#include "raylib.h"
#include "tree.h"
#include "utils.c"

#define WIN_HEIGHT 1600
#define WIN_WIDTH 800

double zoom_factor = 1.0;
Vector2 map_cartesian_screen(Vector2 coords) {
    Vector2 screen_coords = {0};
    screen_coords.x = zoom_factor * coords.x + (float)GetScreenWidth() / 2;
    screen_coords.y = (float)GetScreenHeight() / 2 - zoom_factor * coords.y;
    return screen_coords;
}

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(WIN_HEIGHT, WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.rotation = 0.0f;
    camera.zoom = 0.3f;
    camera.offset =
        (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    Vector2 temp_cam;
    int mouse_down = 0;

    // Body* body_arr = load_values_from_file("../src/planet.dat");
    // Body* body_arr = body_init();
    Body* body_arr = init_cluster_bodies();
    if (body_arr == NULL) {
        return -1;
    }

    BoundingBox bb = {(Vector3){.x = -5000, .y = -5000},
                      (Vector3){.x = 5000, .y = 5000}};

    struct Quadtree* qTree = createTree(bb);

    while (!WindowShouldClose()) {
        for (size_t i = 0; i < MAX_BODIES; i++) {
            update_body_positon(&body_arr[i]);
        }
        for (size_t i = 0; i < MAX_BODIES; i++) {
            insertBody(qTree, &body_arr[i], i);
        }

        updateMass(qTree);

        for (size_t i = 0; i < MAX_BODIES; i++) {
            body_arr[i].prev_accel = body_arr[i].acceleration;
            body_arr[i].acceleration=(Vector3){0,0,0};
            updateForce(qTree, &body_arr[i], i);
        }
        // compute_body_force(body_arr);

        for (size_t i = 0; i < MAX_BODIES; i++) {
            update_body_velocity(&body_arr[i]);
        }
        

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            temp_cam = (Vector2){GetMouseX(), GetMouseY()};
            mouse_down = 1;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouse_down) {
            camera.target = temp_cam;
            mouse_down = 0;
        }

        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        Vector2 fpsPos = GetScreenToWorld2D((Vector2){5, 5}, camera);
        DrawFPS(fpsPos.x, fpsPos.y);

        int treeSize = getTreeSize(qTree);
        char str[24];
        sprintf(str, "%d", treeSize);
        Vector2 sizePos = GetScreenToWorld2D((Vector2){20, 20}, camera);
        DrawText(str, sizePos.x, sizePos.y, 40 + 1 / camera.zoom, GREEN);

        for (size_t i = 0; i < MAX_BODIES; i++) {
            // Vector2 mapCorrd = map_cartesian_screen((Vector2){
            //     .x = body_arr[i].position.x, .y = body_arr[i].position.y});
            // DrawCircleV(mapCorrd, i == 0 ? 50 : 1, body_arr[i].shape.color);
            DrawCircleV(
                (Vector2){body_arr[i].position.x, body_arr[i].position.y},
                i == 0 ? 100 : 5, body_arr[i].shape.color);
        }
        //DebugQuadTree(qTree);
        EndMode2D();
        EndDrawing();

        deleteTree(qTree);
        qTree = createTree(bb);
    }
    free(body_arr);
    body_arr = NULL;
    CloseWindow();
}