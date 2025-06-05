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
   mass of all its children bodies if certain ratio > a where a is a threshold.
*/

typedef enum {
    NORTH_WEST = (1 << 0),
    NORTH_EAST = (1 << 1),
    SOUTH_WEST = (1 << 2),
    SOUTH_EAST = (1 << 3)
} QuadType;

// Keeping only center of bounding box, square magic onwards
struct Quadtree {
    float mass;
    Vector2 body_pos;
    // indexes into the body_arr, -1 if no body present in a node, -2 if its an
    // internal node
    int index;

    struct Quadtree* nw;
    struct Quadtree* ne;
    struct Quadtree* se;
    struct Quadtree* sw;
};

bool BoxContainsPoint(float, float, float, float, float);
Vector2 returnSubQuad(float, float, float, QuadType);
// bool checkBodyInQuad(BoundingBox ,QuadType);
struct Quadtree* createTree();
bool insertBody(struct Quadtree*, float, float, float, Vector2, float, int,
                int depth);
void QuadSubDivide(struct Quadtree*);
void deleteTree(struct Quadtree*);
void updateForce(struct Quadtree*, Body*, int, int, float);
void updateMass(struct Quadtree*);
void DebugQuadTree(struct Quadtree*, float, float, float, char);
int getTreeSize(struct Quadtree*);

#endif /* tree.h */
