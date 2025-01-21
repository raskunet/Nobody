#include "tree.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

#include "body.h"
#include "raylib.h"
#include "vector.h"

bool BoxContainsPoint(BoundingBox bb, float x, float y) {
    if (bb.min.x <= x && bb.max.x >= x && bb.min.y <= y && bb.max.y >= y)
        return true;
    return false;
}

float BoundingBoxLength(BoundingBox bb) { return fabs(bb.min.x - bb.max.x); }

BoundingBox returnSubQuad(BoundingBox bb, QuadType quadT) {
    BoundingBox subBB = {0, 0, 0, 0};
    float bbLength = BoundingBoxLength(bb);
    if (quadT & NORTH_WEST) {
        subBB.min = bb.min;
        subBB.max = (Vector3){bb.min.x + bbLength / 2, bb.min.y + bbLength / 2};
    }
    if (quadT & NORTH_EAST) {
        subBB.min = (Vector3){bb.min.x, bb.min.y + bbLength / 2, 0};
        subBB.max = (Vector3){bb.min.x + bbLength / 2, bb.max.y, 0};
    }
    if (quadT & SOUTH_EAST) {
        subBB.min = (Vector3){bb.min.x + bbLength / 2, bb.min.y + bbLength / 2};
        subBB.max = bb.max;
    }
    if (quadT & SOUTH_WEST) {
        subBB.min = (Vector3){bb.min.x + bbLength / 2, bb.min.y};
        subBB.max = (Vector3){bb.max.x, bb.min.y + bbLength / 2};
    }
    return subBB;
}

struct Quadtree* createTree(BoundingBox bb) {
    struct Quadtree* qTree =
        (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));
    qTree->AABB = bb;
    qTree->com_mass = 0;
    qTree->centre_of_mass = (Vector3){0, 0, 0};
    qTree->body = NULL;
    qTree->nw = NULL;
    qTree->ne = NULL;
    qTree->sw = NULL;
    qTree->se = NULL;
    return qTree;
}

void QuadSubDivide(struct Quadtree* qTree) {
    qTree->nw = (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));
    qTree->ne = (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));
    qTree->sw = (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));
    qTree->se = (struct Quadtree*)MemAlloc(sizeof(struct Quadtree));

    qTree->nw->AABB = returnSubQuad(qTree->AABB, NORTH_WEST);
    qTree->ne->AABB = returnSubQuad(qTree->AABB, NORTH_EAST);
    qTree->sw->AABB = returnSubQuad(qTree->AABB, SOUTH_WEST);
    qTree->se->AABB = returnSubQuad(qTree->AABB, SOUTH_EAST);
}

/*
    External Node will have no childrens but contains 1 or 0 bodies.
    Internal Nodes will have atmost 4 childrens and represent information about
    group of bodies beneath them.
*/
bool insertBody(struct Quadtree* qTree, Body* body, int index) {
    if (!BoxContainsPoint(qTree->AABB, body->position.x, body->position.y)) {
        return false;
    }

    // If a node doesn't contain a body, insert it here.
    if (qTree->body == NULL) {
        qTree->body = body;
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
        insertBody(qTree, qTree->body, qTree->index);
        insertBody(qTree, body, index);
        return true;
    }

    // Update the Internal Node Centre of mass and total mass
    // qTree->body = add(qTree->body, body);
    // MemFree(qTree->body);

    // Now insert the point into the newly created Childrens.
    if (insertBody(qTree->nw, body, index)) return true;
    if (insertBody(qTree->ne, body, index)) return true;
    if (insertBody(qTree->sw, body, index)) return true;
    if (insertBody(qTree->se, body, index)) return true;

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
void DebugQuadTree(struct Quadtree* qTree) {
    if (qTree) {
        DrawBoundingBox(qTree->AABB, (Color){2, 88, 13, 255});
        DebugQuadTree(qTree->nw);
        DebugQuadTree(qTree->ne);
        DebugQuadTree(qTree->sw);
        DebugQuadTree(qTree->se);
    }
}
/*
    Calculate Force
    Take qTree->body as A
    body as B
*/
void updateForce(struct Quadtree* qTree, Body* body, int index) {
    if (qTree) {
        if (qTree->index != index) {
            if (qTree->nw == NULL && qTree->ne == NULL && qTree->se == NULL &&
                qTree->sw == NULL) {
                calculate_net_force(body, qTree->body);
            } else {
                float ratio =
                    (BoundingBoxLength(qTree->AABB) /
                     (eucld_dist(qTree->body->position, body->position)));
                if (ratio <= 0.5) {
                    calculate_net_force(
                        body, &(Body){.mass = qTree->com_mass,
                                      .position = qTree->centre_of_mass,
                                      .acceleration = (Vector3){
                                          0,
                                          0,
                                          0,
                                      }});
                } else {
                    updateForce(qTree->nw, body, index);
                    updateForce(qTree->ne, body, index);
                    updateForce(qTree->sw, body, index);
                    updateForce(qTree->se, body, index);
                }
            }
        }
    }
}

void updateMass(struct Quadtree* qTree) {
    if (qTree) {
        if (qTree->body == NULL) {
            return;
        }
        if (qTree->nw == NULL && qTree->ne == NULL && qTree->se == NULL &&
            qTree->sw == NULL) {
            qTree->body = qTree->body;
            qTree->com_mass = qTree->body->mass;
            qTree->centre_of_mass=qTree->body->position;
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
        qTree->com_mass = qTree->nw->com_mass + qTree->ne->com_mass +
                          qTree->sw->com_mass + qTree->se->com_mass;
        qTree->centre_of_mass.x =
            qTree->nw->centre_of_mass.x * qTree->nw->com_mass +
            qTree->ne->centre_of_mass.x * qTree->ne->com_mass +
            qTree->sw->centre_of_mass.x * qTree->sw->com_mass +
            qTree->se->centre_of_mass.x * qTree->se->com_mass;

        qTree->centre_of_mass.x /= qTree->com_mass;

        qTree->centre_of_mass.y =
            qTree->nw->centre_of_mass.y * qTree->nw->com_mass +
            qTree->ne->centre_of_mass.y * qTree->ne->com_mass +
            qTree->sw->centre_of_mass.y * qTree->sw->com_mass +
            qTree->se->centre_of_mass.y * qTree->se->com_mass;

        qTree->centre_of_mass.y /= qTree->com_mass;
        
    }
}

int getTreeSize(struct Quadtree* qTree) {
    if (qTree && qTree->body != NULL) {
        int sum = getTreeSize(qTree->ne) + getTreeSize(qTree->nw) +
                  getTreeSize(qTree->se) + getTreeSize(qTree->sw);
        return sum;
    }
    return 1;
}