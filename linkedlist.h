#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <string.h>
#include <math.h>

typedef struct node {
    int shape;
    void *data;
    struct node *prev;
    struct node *next;
} Node;

typedef struct pt {
    bool show;
    GLfloat xPos;
    GLfloat yPos;
    GLfloat *col;
} Pt;

typedef struct line {
    Pt *p1;
    Pt *p2;
    GLfloat *col;
    bool dashed;
    bool fill;
} Line;

typedef struct triangle {
    Pt *v1;
    Pt *v2;
    Pt *v3;
    GLfloat* col;
    int dashed;
    bool fill;
} Triangle;

typedef struct rect {
    Pt *v1;
    Pt *v2;
    Pt *v3;
    Pt *v4;
    GLfloat *col;
    bool dashed;
    bool fill;
} Rect;

typedef struct circle {
    Pt *v1;
    Pt *v2;
    Pt *center;
    GLfloat radius;
    GLfloat *col;
    GLfloat *xInd;
    GLfloat *yInd;
    bool dashed;
    bool fill;
} Circle;

typedef struct polygon {
    Pt *v1;
    Pt *v2;
    Pt *v3;
    Pt *v4;
    double *data;
    GLfloat *col;
    bool dashed;
    bool fill;
} Polygon;

void clearAll(Node *head);
Node *initLinkedList();
Node *makeNode(int shape, void *data);
void addNode(Node *tail, Node *newPrim);
Pt *makePt(GLfloat xPos, GLfloat yPos, GLfloat colors[]);
Line *makeLine(Pt *xPos, Pt *yPos, GLfloat colors[]);
Triangle *makeTriangle(Pt *p1, Pt *p2, Pt *p3, GLfloat colors[]);
Rect *makeRect(Pt* p1, Pt* p2, GLfloat colors[]);
Circle *makeCircle(Pt *d1, Pt *d2, GLfloat colors[]);
bool lineFree(Node *t);
bool triangleFree(Node *tail);
bool rectFree(Node* tail);
bool circleFree(Node* tail);

void freeAll(Node* head);
void freePrim(Node *prim);

#endif
