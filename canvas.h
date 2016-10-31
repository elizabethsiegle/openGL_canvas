#ifndef CANVAS_H
#define CANVAS_H

#include "linkedlist.h"
#include <math.h>
#include <string.h>
#ifdef __APPLE__
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif

#define WIDTH 800
#define HEIGHT 600
#define POINT 1
#define LINE 2
#define TRIANGLE 3
#define RECT 4
#define CIRCLE 5

void init();
void cursor(GLFWwindow *window, GLdouble xpos, GLdouble ypos);
void reshape(GLFWwindow *wind, float w, float h);
void framebuffer_resize(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *w, int key, int scancode, int action, int mods);
void mouse(GLFWwindow* window, int button, int action, int mods);
void cursor(GLFWwindow* window, GLdouble x, GLdouble y);
void checkRectPts(GLdouble *res, GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
void checkCircleIndices(Circle *c);
void drawPt(Pt *p);
void drawRect(Rect* r);
void switchCol(GLfloat *target, GLfloat *colorClicked);
void drawVertices(double *arr, int startIndex, int leng);
void checkCircleIndices(Circle *c);
GLfloat circRadius(Pt *center, Pt *edge);
bool hasCircle(Primitive *tail);
bool hasRect(Primitive *tail);
bool hasTriangle(Primitive *tail);
bool hasLine(Primitive *tail);
void clickedPt();
void removePrev(Primitive *prim);
void mouseDownCircle();
void mouseDownLine();
void mouseDownRect();
void mouseDownTriangle();
void mouseUpTriangle();
void mouseUpRect();
void mouseUpCircle();
void cursorCircle();
void cursorRect();
void cursorTri();
void cursorLine();
bool circleAvail(Circle *c);
bool lineAvail(Line *l);
bool triangleAvail(Triangle *t);
void freePrim(Primitive* prim);

#endif
