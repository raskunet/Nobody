#ifndef TREE_H
#define TREE_H

#include "body.h"
#include "raylib.h"

/*
    |--------Quad----------|

    |----------|-----------|
    |   NW     |   NE      |
    |          |           |
    |__________|___________|
    |          |           |
    |    SW    |   SE      |
    |__________|___________|


    Each external node represents a single body. Each internal node represents
   the group of bodies beneath it, and stores the center-of-mass and the total
   mass of all its children bodies.
*/

typedef enum {
    NORTH_WEST = (1 << 0),
    NORTH_EAST = (1 << 1),
    SOUTH_WEST = (1 << 2),
    SOUTH_EAST = (1 << 3)
} QuadType;

// typedef struct QUADTREE Quadtree;
struct Quadtree {
    BoundingBox AABB;
    Body* body;
    struct Quadtree* nw;
    struct Quadtree* ne;
    struct Quadtree* se;
    struct Quadtree* sw;
};

bool BoxContainsPoint(BoundingBox, float, float);
float BoundingBoxLength(BoundingBox);
BoundingBox returnSubQuad(BoundingBox, QuadType);
// bool checkBodyInQuad(BoundingBox ,QuadType);
struct Quadtree * createTree(BoundingBox );
bool insertBody(struct Quadtree*, Body*);
void QuadSubDivide(struct Quadtree* );
void deleteTree(struct Quadtree*);

#endif /* tree.h */
