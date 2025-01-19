#include "tree.h"

#include <math.h>
#include <stddef.h>

#include "body.h"
#include "raylib.h"

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
    qTree->body = NULL;
    qTree->ne = NULL;
    qTree->nw = NULL;
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
bool insertBody(struct Quadtree* qTree, Body* body) {
    if (!BoxContainsPoint(qTree->AABB, body->position.x, body->position.y)) {
        return false;
    }

    // If a node doesn't contain a body, insert it here.
    if (qTree->body == NULL && qTree->nw == NULL) {
        qTree->body = body;
        return true;
    }

    // This converts external node to internal with four childrens.
    // Now this one becomes internal node
    if (qTree->nw == NULL) {
        QuadSubDivide(qTree);
    }

    // Update the Internal Node Centre of mass and total mass 
    qTree->body = add(qTree->body, body);

    // Now insert the point into the newly created Childrens.
    if (insertBody(qTree->nw, body)) return true;
    if (insertBody(qTree->ne, body)) return true;
    if (insertBody(qTree->sw, body)) return true;
    if (insertBody(qTree->se, body)) return true;

    return false;
}

void deleteTree(struct Quadtree* qTree) {
    if (!qTree) {
        deleteTree(qTree->nw);
        deleteTree(qTree->ne);
        deleteTree(qTree->sw);
        deleteTree(qTree->se);
        MemFree(qTree);
    }
    return;
}