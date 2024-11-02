#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "body.h"
#include "raylib.h"
#include "utils.c"

#define MIN_WIN_HEIGHT 1920
#define MIN_WIN_WIDTH 1080

double zoom_factor = 1.0e-1f;
Vector2 map_cartesian_screen(Vector2 coords) {
    Vector2 screen_coords = {0};
    screen_coords.x = zoom_factor * coords.x + (float)GetScreenWidth() / 2;
    screen_coords.y = (float)GetScreenHeight() / 2 - zoom_factor * coords.y;
    return screen_coords;
}

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(MIN_WIN_HEIGHT, MIN_WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.offset =
        (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 0.7f;

    Vector2 temp_cam;
    int mouse_down = 0;
    int angle = 0;
    int frame_counter = 0;

    // Body* body_arr = load_values_from_file("../src/planet.dat");
    //Body* body_arr = body_init();
    Body* body_arr = init_cluster_bodies();
    if (body_arr == NULL) {
        return -1;
    }

    while (!WindowShouldClose()) {
        for (size_t i = 0; i < MAX_BODIES; i++) {
            update_body_positon(&body_arr[i]);
        }

        compute_body_force(body_arr);

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
        DrawFPS(5, 5);
        for (size_t i = 0; i < MAX_BODIES; i++) {
            Vector2 mapCorrd = map_cartesian_screen((Vector2){
                .x = body_arr[i].position.x, .y = body_arr[i].position.y});
            DrawCircleV(mapCorrd, i == 0 ? 10 : 5, body_arr[i].shape.color);
        }
        EndMode2D();
        EndDrawing();
    }
    free(body_arr);
    body_arr = NULL;
    CloseWindow();
}