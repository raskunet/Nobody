#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "body.h"
#include "raylib.h"
#include "tree.h"
#include "utils.c"

#define WIN_HEIGHT 1600
#define WIN_WIDTH 800

Vector2 bb_l = {-10000.0, -10000.0}, bb_r = {5000.0, 5000.0};
double zoom_factor = 1.0;
Vector2 map_cartesian_screen(Vector2 coords) {
    Vector2 screen_coords = {0};
    screen_coords.x = zoom_factor * coords.x + (float)GetScreenWidth() / 2;
    screen_coords.y = (float)GetScreenHeight() / 2 - zoom_factor * coords.y;
    return screen_coords;
}

void update_bodies(Body* body_arr, struct Quadtree* qTree) {
    for (size_t i = 0; i < MAX_BODIES; i++) {
        update_body_positon(&body_arr[i]);
    }

    for (size_t i = 0; i < MAX_BODIES; i++) {
        insertBody(
            qTree, (bb_l.x + bb_r.x) / 2.0, (bb_l.y + bb_r.y) / 2.0,
            fabsf(bb_l.x - bb_r.x),
            (Vector2){.x = body_arr[i].position.x, body_arr[i].position.y},
            body_arr[i].mass, i, 128);
    }
    updateMass(qTree);

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i].prev_accel = body_arr[i].acceleration;
        body_arr[i].acceleration = (Vector3){0, 0, 0};
        updateForce(qTree, &body_arr[i], i, 0, fabsf(bb_l.x - bb_r.x));
    }
    // compute_body_force(body_arr);

    for (size_t i = 0; i < MAX_BODIES; i++) {
        update_body_velocity(&body_arr[i]);
    }
}

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(WIN_HEIGHT, WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.rotation = 0.0f;
    camera.zoom = 0.1f;
    camera.offset =
        (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    Vector2 temp_cam;
    int mouse_down = 0;
    bool show_tree = 0;

    // Body* body_arr = load_values_from_file("../src/planet.dat");
    // Body* body_arr = body_init();
    //Body* body_arr = init_cluster_bodies();
    Body* body_arr = init_colliding_galaxies();
    if (body_arr == NULL) {
        return -1;
    }

    struct Quadtree* qTree = createTree();

    while (!WindowShouldClose()) {
        update_bodies(body_arr, qTree);

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            temp_cam = (Vector2){GetMouseX(), GetMouseY()};
            mouse_down = 1;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouse_down) {
            camera.target = temp_cam;
            mouse_down = 0;
        }
        if (IsKeyReleased(KEY_TAB)) {
            show_tree = !show_tree;
        }

        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        // if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        // if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        int treeSize = getTreeSize(qTree);
        char str[128];
        sprintf(str, "FPS: %d\n\n\n\n\n\n\nNodes: %d", GetFPS(),treeSize);
        Vector2 sizePos = GetScreenToWorld2D((Vector2){0, 0}, camera);
        DrawText(str, sizePos.x, sizePos.y, 40 + 11 * (1 / camera.zoom), GREEN);

        for (size_t i = 0; i < MAX_BODIES; i++) {
            // Vector2 mapCorrd = map_cartesian_screen((Vector2){
            //     .x = body_arr[i].position.x, .y = body_arr[i].position.y});
            // DrawCircleV(mapCorrd, i == 0 ? 50 : 1, body_arr[i].shape.color);

            // Small hack when simulating two galaxies :)
            DrawCircleV(
                (Vector2){body_arr[i].position.x, body_arr[i].position.y},
                (i == 0 || i == 1) ? 50 : 5, body_arr[i].color);
        }
        if (show_tree) {
            DebugQuadTree(qTree, -10000, -10000, fabsf(bb_l.x - bb_r.x), '0');
        }
        EndMode2D();
        EndDrawing();

        deleteTree(qTree);
        qTree = createTree();
    }
    free(body_arr);
    body_arr = NULL;
    CloseWindow();
}