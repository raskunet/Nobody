#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"
#include "utils.c"
#define MIN_WIN_HEIGHT GetMonitorHeight(1)
#define MIN_WIN_WIDTH GetMonitorWidth(1)

double zoom_factor = .005f;
Vector2 map_cartesian_screen(Vector2 coords) {
    Vector2 screen_coords = {0};
    screen_coords.x = zoom_factor * coords.x + (float)GetScreenWidth() / 2;
    screen_coords.y = (float)GetScreenHeight() / 2 - zoom_factor * coords.y;
    return screen_coords;
}

Body* body_init() {
    srand(time(NULL));
    Body* body_arr = (Body*)MemAlloc(MAX_BODIES * sizeof(Body));
    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i] = rand_body();
    }

    Vector3 v_sum = {0}, p_sum = {0};
    double mass_sum = 0;
    for (size_t i = 0; i < MAX_BODIES; i++) {
        v_sum = add_vector(v_sum, body_arr[i].velocity);
        p_sum = add_vector(p_sum, body_arr[i].position);
        mass_sum += body_arr[i].mass;
    }

    Vector3 v_avg = div_vector(mass_sum, v_sum),
            p_avg = div_vector(mass_sum, p_sum);

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i].velocity = subtract_vector(body_arr[i].velocity, v_avg);
        body_arr[i].position = subtract_vector(body_arr[i].position, p_avg);
    }

    double max_r = -DBL_MAX;

    for (size_t i = 0; i < MAX_BODIES; i++) {
        double dist_r = mag_vector(body_arr[i].position);
        if (dist_r > max_r) {
            max_r = dist_r;
        }
    }

    for (size_t i = 0; i < MAX_BODIES; i++) {
        body_arr[i].position = div_vector(max_r, body_arr[i].position);
    }

    return body_arr;
}

int main() {
    SetRandomSeed(time(NULL));
    InitWindow(MIN_WIN_HEIGHT, MIN_WIN_WIDTH, "N Body Simulation");
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.offset =
        (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    camera.rotation = 0.0f;

    Vector2 temp_cam;
    int mouse_down = 0;
    int angle = 0;
    int frame_counter = 0;

    double soft_fact = 0.98f * pow(MAX_BODIES, -0.26);
    double mise_opt = 0.22 * pow(MAX_BODIES, -0.68);

    Body* body_arr = body_init();
    // load_values_from_file("../src/planet.dat");   body_init();

    if (body_arr == NULL) {
        return -1;
    }

    while (!WindowShouldClose()) {
        for (size_t i = 0; i < MAX_BODIES; i++) {
            Vector3 accel_i = {0};
            double mass = body_arr[i].mass;
            for (size_t j = 0; j < MAX_BODIES; j++) {
                if (i != j) {
                    Vector3 dist = subtract_vector(body_arr[j].position,
                                                   body_arr[i].position);
                    if (fabs(dist.x) < EPSILON || fabs(dist.y) < EPSILON) {
                        break;
                    }
                    double mag_r = dist.x * dist.x + dist.y * dist.y;
                    double mag_r_sqrt = sqrt(mag_r);
                    double a = (G_CONST * body_arr[j].mass) /
                               pow((mag_r * mag_r_sqrt + soft_fact), 1.5);
                    Vector3 temp;
                    // temp.x = dist.x / (fmax(mag_r_sqrt, DBL_MIN) * mag_r +
                    //                    pow(soft_fact, 2));
                    // temp.y = dist.y / (fmax(mag_r_sqrt, DBL_MIN) * mag_r +
                    //                    pow(soft_fact, 2));

                    // temp.x =
                    //     dist.x /
                    //     pow((mag_r_sqrt * mag_r_sqrt + pow(soft_fact,
                    //     2)), 1.5);
                    // temp.y =
                    //     dist.y /
                    //     pow((mag_r_sqrt * mag_r_sqrt + pow(soft_fact,
                    //     2)), 1.5);

                    accel_i.x += a * dist.x;
                    accel_i.y += a * dist.y;
                    // accel_i.x += G_CONST* body_arr[j].mass * temp.x;
                    // accel_i.y += G_CONST* body_arr[j].mass * temp.y;
                }
            }
            body_arr[i].velocity.x += accel_i.x * dt;
            body_arr[i].velocity.y += accel_i.y * dt;
            // printf(
            //     "Body[%ld] Velocity is vx = %f vy =%f\nAcceleration is ax=%f\
            // "
            //     "ay=%f\n",
            //     i, body_arr[i].velocity.x, body_arr[i].velocity.y, accel_i.x,
            //     accel_i.y);
        }
        for (size_t i = 0; i < MAX_BODIES; i++) {
            body_arr[i].position.x += body_arr[i].velocity.x * dt;
            body_arr[i].position.y += body_arr[i].velocity.y * dt;
            // printf("Position is x=%f, y=%f\n", body_arr[i].position.x,
            //        body_arr[i].position.y);
        }
        t += dt;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            temp_cam = (Vector2){GetMouseX(), GetMouseY()};
            mouse_down = 1;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouse_down) {
            camera.target = temp_cam;
            mouse_down = 0;
        }

        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        // if (camera.zoom > 3.0f)
        //     camera.zoom = 3.0f;
        if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        BeginDrawing();
        ClearBackground(BLACK);
        // BeginMode2D(camera);
        //  DrawFPS(5, 5);
        for (size_t i = 0; i < MAX_BODIES; i++) {
            Vector2 mapCorrd = map_cartesian_screen((Vector2){
                .x = body_arr[i].position.x, .y = body_arr[i].position.y});
            printf("MapCoords body [%ld] X=%f, y=%f\n", i, mapCorrd.x,
                   mapCorrd.y);
            // Vector2 mapCorrd = (Vector2){.x = body_arr[i].position.x,
            //                              .y = body_arr[i].position.y};
            DrawCircleV(mapCorrd, 1, body_arr[i].shape.color);
        }
        // EndMode2D();
        EndDrawing();
    }
    free(body_arr);
    body_arr = NULL;
    CloseWindow();
}