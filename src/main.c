#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "utils.c"
#define MIN_WIN_HEIGHT GetMonitorHeight(1)
#define MIN_WIN_WIDTH GetMonitorWidth(1)

double zoom_factor = 100.0f;
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

Body* init_cluster_bodies() {
    srand(time(NULL));
    Body* body_arr = (Body*)MemAlloc(MAX_BODIES * sizeof(Body));
    // set origin
    body_arr[0].mass = 10;
    body_arr[0].position = (Vector3){0, 0, 0};
    body_arr[0].velocity = (Vector3){
        0,
        0,
        0,
    };
    body_arr[0].shape.color = YELLOW;

    float nominal_mass = 0.1;
    double total_mass = body_arr[0].mass;
    for (int i = 1; i < MAX_BODIES; i++) {
        body_arr[i].mass = (rand() / (float)RAND_MAX) * nominal_mass;
        total_mass += body_arr[i].mass;
        body_arr[i].shape.color = ORANGE;
    }
    for (int i = 1; i < MAX_BODIES; i++) {
        double r = 1000 * sqrt((rand()) / (float)RAND_MAX);
        double theta = (rand() / (double)RAND_MAX) * 2 * PI;

        body_arr[i].position.x = cos(theta) * r;
        body_arr[i].position.y = sin(theta) * r;
        body_arr[i].position.z = 0.0;

        r = sqrtf(G_CONST * total_mass / r) * (rand() / (double)RAND_MAX + 3) /
            4;
        theta += M_PI / 2 + (rand() / (double)RAND_MAX - 0.5) * M_PI / 2;

        // body_arr[i].velocity.x = cos(theta) * r;
        // body_arr[i].velocity.y = cos(theta) * r;
        // body_arr[i].velocity.z = 0.0;
        //(rand() / (float)RAND_MAX - 0.5) * 10000.0;)
        body_arr[i].velocity.x = (rand() / (float)RAND_MAX - 0.5) * 100.0f;
        body_arr[i].velocity.y = (rand() / (float)RAND_MAX - 0.5) * 100.0f;
        body_arr[i].velocity.z = 0.0;
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
    camera.zoom = 0.7f;

    Vector2 temp_cam;
    int mouse_down = 0;
    int angle = 0;
    int frame_counter = 0;

    double soft_fact = 0.98f * pow(MAX_BODIES, -0.26);
    double mise_opt = 0.22f * pow(MAX_BODIES, -0.68);
    double ESP2 = pow(1e-2f, 2);

    // Body* body_arr = load_values_from_file("../src/planet.dat");
    Body* body_arr = body_init();
    //Body* body_arr = init_cluster_bodies();
    if (body_arr == NULL) {
        return -1;
    }

    while (!WindowShouldClose()) {

        for (size_t i = 0; i < MAX_BODIES; i++) {
            Vector3 accel_prev = body_arr[i].acceleration;
            body_arr[i].acceleration = (Vector3){0, 0, 0};
            double mass = body_arr[i].mass;
            for (size_t j = i+1; j < MAX_BODIES; j++) {
                if (j != i) {
                    Vector3 dist = subtract_vector(body_arr[j].position,
                                                   body_arr[i].position);
                    
                    double mag_r = dist.x * dist.x + dist.y * dist.y + ESP2;
                    if (mag_r > 100000) {
                        mag_r = 100000;
                    } else if (mag_r < 10) {
                        mag_r = 10;
                    }
                    float distSixth = mag_r * mag_r * mag_r;

                    float invDistCube = 1.0f * (1 / sqrt(distSixth));

                    float f = mass * invDistCube;


                    body_arr[i].acceleration.x += G_CONST * f * dist.x;
                    body_arr[i].acceleration.y += G_CONST * f * dist.y;
                }
                
                //     printf(
                //         "Body[%ld] Velocity is vx = %f vy
                //=%f\nAcceleration
                //         is ax=%f " "ay=%f\n", i, body_arr[i].velocity.x,
                //         body_arr[i].velocity.y,
                // body_arr[i].acceleration.x,
                //         body_arr[i].acceleration.y);
                // }
            }
            body_arr[i].velocity.x +=
                0.5 * (accel_prev.x + body_arr[i].acceleration.x) * dt;
            body_arr[i].velocity.y +=
                0.5 * (accel_prev.y + body_arr[i].acceleration.y) * dt;
        }
        t += dt;
        for (size_t i = 0; i < MAX_BODIES; i++) {
            body_arr[i].position.x +=
                body_arr[i].velocity.x * dt +
                0.5 * (body_arr[i].acceleration.x) * (dt * dt);
            body_arr[i].position.y +=
                body_arr[i].velocity.y * dt +
                0.5 * (body_arr[i].acceleration.y) * (dt * dt);
            ;
            // printf("Position is x=%f, y=%f\n",
            // body_arr[i].position.x,
            //        body_arr[i].position.y);
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

        if (camera.zoom > 3.0f)
             camera.zoom = 3.0f;
        if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        DrawFPS(5, 5);
        for (size_t i = 0; i < MAX_BODIES; i++) {
            Vector2 mapCorrd = map_cartesian_screen((Vector2){
                .x = body_arr[i].position.x, .y = body_arr[i].position.y});
            // printf("MapCoords body [%ld] X=%f, y=%f\n", i,
            // mapCorrd.x,
            //        mapCorrd.y);
            DrawCircleV(mapCorrd, 1, body_arr[i].shape.color);
        }
        EndMode2D();
        EndDrawing();
    }
    free(body_arr);
    body_arr = NULL;
    CloseWindow();
}