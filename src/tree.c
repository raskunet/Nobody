#include "tree.h"

#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "vector.h"

bool BoxContainsPoint(float cx, float cy, float length, float x, float y) {
    float lx = cx - length / 2.0;
    float ly = cy - length / 2.0;
    float bx = cx + length / 2.0;
    float by = cy + length / 2.0;

    if (x >= lx && y >= ly && x <= bx && y <= by) return true;

    return false;
}

Vector2 returnSubQuad(float length, float cx, float cy, QuadType quadT) {
    Vector2 subBB;
    float nx, ny;
    if (quadT & NORTH_WEST) {
        nx = cx - length / 2.0;
        ny = cy - length / 2.0;
    }
    if (quadT & NORTH_EAST) {
        nx = cx - length / 2.0;
        ny = cy + length / 2.0;
    }
    if (quadT & SOUTH_WEST) {
        nx = cx + length / 2.0;
        ny = cy - length / 2.0;
    }
    if (quadT & SOUTH_EAST) {
        nx = cx + length / 2.0;
        ny = cy + length / 2.0;
    }
    subBB.x = (cx + nx) / 2.0;
    subBB.y = (cy + ny) / 2.0;
    return subBB;
}

struct Quadtree* createTree() {
    struct Quadtree* qTree =
        (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));
    qTree->mass = 0;
    qTree->body_pos = (Vector2){0, 0};
    qTree->index = -1;
    qTree->nw = NULL;
    qTree->ne = NULL;
    qTree->sw = NULL;
    qTree->se = NULL;
    return qTree;
}

void QuadSubDivide(struct Quadtree* qTree) {
    qTree->nw = createTree();
    qTree->ne = createTree();
    qTree->sw = createTree();
    qTree->se = createTree();

    // qTree->nw->AABB = returnSubQuad(qTree->AABB, NORTH_WEST);
    // qTree->ne->AABB = returnSubQuad(qTree->AABB, NORTH_EAST);
    // qTree->sw->AABB = returnSubQuad(qTree->AABB, SOUTH_WEST);
    // qTree->se->AABB = returnSubQuad(qTree->AABB, SOUTH_EAST);
}

/*
    External Node will have no childrens but contains 1 or 0 bodies.
    Internal Nodes will have atmost 4 childrens and represent information about
    group of bodies beneath them.
*/
bool insertBody(struct Quadtree* qTree, float c_x, float c_y, float length,
                Vector2 position, float mass, int index, int depth) {
    if (depth == 0) {
        return false;
    }
    if (!BoxContainsPoint(c_x, c_y, length, position.x, position.y)) {
        return false;
    }

    // If a node doesn't contain a body, insert it here.
    if (qTree->index == -1) {
        qTree->body_pos = position;
        qTree->mass = mass;
        qTree->index = index;
        return true;
    }

    // This converts external node to internal with four childrens.
    // Now this one becomes internal node
    if (qTree->nw == NULL && qTree->ne == NULL && qTree->se == NULL &&
        qTree->sw == NULL) {
        QuadSubDivide(qTree);
        // qTree->body = add(qTree->body, body);
        // Body* old_body = qTree->body;
        // insertBody(qTree, qTree->body, qTree->index, depth);
        // insertBody(qTree, body, index, depth - 1);

        insertBody(qTree, c_x, c_y, length, qTree->body_pos, qTree->mass,
                   qTree->index, depth);
        insertBody(qTree, c_x, c_y, length, position, mass, index, depth - 1);
        return true;
    }

    // Update the Internal Node Centre of mass and total mass
    // qTree->body = add(qTree->body, body);
    // MemFree(qTree->body);

    // Now insert the point into the newly created Childrens.
    Vector2 subBB = returnSubQuad(length, c_x, c_y, NORTH_WEST);
    if (insertBody(qTree->nw, subBB.x, subBB.y, length / 2, position, mass,
                   index, depth - 1))
        return true;

    subBB = returnSubQuad(length, c_x, c_y, NORTH_EAST);
    if (insertBody(qTree->ne, subBB.x, subBB.y, length / 2, position, mass,
                   index, depth - 1))
        return true;

    subBB = returnSubQuad(length, c_x, c_y, SOUTH_WEST);
    if (insertBody(qTree->sw, subBB.x, subBB.y, length / 2, position, mass,
                   index, depth - 1))
        return true;

    subBB = returnSubQuad(length, c_x, c_y, SOUTH_EAST);
    if (insertBody(qTree->se, subBB.x, subBB.y, length / 2, position, mass,
                   index, depth - 1))
        return true;

    return false;
}

void deleteTree(struct Quadtree* qTree) {
    if (qTree) {
        deleteTree(qTree->nw);
        deleteTree(qTree->ne);
        deleteTree(qTree->sw);
        deleteTree(qTree->se);
        // MemFree(qTree->body);
        MemFree(qTree);
    }
    return;
}
void DebugQuadTree(struct Quadtree* qTree, float mx, float my, float length) {
    if (qTree) {
        DrawRectangleLines(mx, my, length, length, (Color){2, 88, 13, 255});
        DebugQuadTree(qTree->nw, mx, my, length / 2);
        DebugQuadTree(qTree->ne, mx, my + length / 2, length / 2);
        DebugQuadTree(qTree->sw, mx + length / 2, my, length / 2);
        DebugQuadTree(qTree->se, mx + length / 2, my + length / 2, length / 2);
    }
}
/*
    Calculate Force
    Take qTree->body as A
    body as B
*/
void updateForce(struct Quadtree* qTree, Body* body, int index, int depth,
                 float length) {
    if (qTree) {
        if (qTree->index != index) {
            if (qTree->nw == NULL && qTree->ne == NULL && qTree->se == NULL &&
                qTree->sw == NULL) {
                calculate_net_force(
                    body, &(Body){.mass = qTree->mass,
                                  .position = (Vector3){.x = qTree->body_pos.x,
                                                        .y = qTree->body_pos.y,
                                                        0.0},
                                  .acceleration = (Vector3){
                                      0,
                                      0,
                                      0,
                                  }});
            } else {
                float sq_l = length / (1 << depth);
                float eu_l = eucld_dist(
                    (Vector3){
                        .x = qTree->body_pos.x, .y = qTree->body_pos.y, 0.0},
                    body->position);

                float ratio = sq_l / eu_l;
                if (ratio <= 0.5) {
                    calculate_net_force(
                        body,
                        &(Body){.mass = qTree->mass,
                                .position = (Vector3){.x = qTree->body_pos.x,
                                                      .y = qTree->body_pos.y,
                                                      0.0},
                                .acceleration = (Vector3){
                                    0,
                                    0,
                                    0,
                                }});
                } else {
                    updateForce(qTree->nw, body, index, depth + 1, length);
                    updateForce(qTree->ne, body, index, depth + 1, length);
                    updateForce(qTree->sw, body, index, depth + 1, length);
                    updateForce(qTree->se, body, index, depth + 1, length);
                }
            }
        }
    }
}

void updateMass(struct Quadtree* qTree) {
    if (qTree) {
        if (qTree->index == -1) {
            return;
        }
        if (qTree->nw == NULL && qTree->ne == NULL && qTree->se == NULL &&
            qTree->sw == NULL) {
            return;
        }
        // Now for internal Nodes calculcate by it's four children
        // recursively

        updateMass(qTree->nw);
        updateMass(qTree->ne);
        updateMass(qTree->sw);
        updateMass(qTree->se);

        // qTree->body = add(qTree->body, qTree->nw->body);
        // qTree->body = add(qTree->body, qTree->ne->body);
        // qTree->body = add(qTree->body, qTree->sw->body);
        // qTree->body = add(qTree->body, qTree->se->body);
        qTree->mass = qTree->nw->mass + qTree->ne->mass + qTree->sw->mass +
                      qTree->se->mass;
        qTree->body_pos.x = qTree->nw->body_pos.x * qTree->nw->mass +
                            qTree->ne->body_pos.x * qTree->ne->mass +
                            qTree->sw->body_pos.x * qTree->sw->mass +
                            qTree->se->body_pos.x * qTree->se->mass;

        qTree->body_pos.x /= qTree->mass;

        qTree->body_pos.y = qTree->nw->body_pos.y * qTree->nw->mass +
                            qTree->ne->body_pos.y * qTree->ne->mass +
                            qTree->sw->body_pos.y * qTree->sw->mass +
                            qTree->se->body_pos.y * qTree->se->mass;

        qTree->body_pos.y /= qTree->mass;
    }
}

int getTreeSize(struct Quadtree* qTree) {
    if (qTree && qTree->index != -1) {
        int sum = getTreeSize(qTree->ne) + getTreeSize(qTree->nw) +
                  getTreeSize(qTree->se) + getTreeSize(qTree->sw);
        return sum;
    }
    return 1;
}