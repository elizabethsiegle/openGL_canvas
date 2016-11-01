#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "canvas.h"
#ifdef __APPLE__
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif

GLdouble lolWidth = WIDTH;
GLdouble lolHeight = HEIGHT;
GLdouble mouseX, mouseY;
GLdouble mouseX2, mouseY2;
GLdouble mouseX3, mouseY3;

GLfloat colors[3] = {0.0, 0.0, 0.0};
GLfloat red[3] = {1, 0, 0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0}; //init
bool redColor, blueColor, greenColor = false;
bool blackColor = true;

bool lineMode = true; //init
bool triangleMode, rectangleMode, circleMode = false;

bool filled = true; //init
bool keyDown, mouseDown = false; //init

Pt *pressedCoords;
Pt *releasedCoords;

Primitive *head;
Primitive *tail;

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0); //white background
    glColor3fv(colors);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT); //0, 0, 250, 250
    glMatrixMode(GL_MODELVIEW);
    glColor3f(0.0, 0.0, 0.0); //black = orig
    glPointSize(10);
    glLineStipple(1, 0x3F07);
}

void framebuffer_resize(GLFWwindow* window, int width, int height) {
    printf("Got Reshape Event\n");
    lolWidth = width;
    lolHeight = height;
    glViewport(0, 0, width, height);
}

GLfloat circRadius(Pt *center, Pt *edge) {
    return sqrt((center->xPos - edge->xPos) * (center->xPos - edge->xPos) + (center->yPos - edge->yPos) * (center->yPos - edge->yPos));
}

void checkCircleIndices(Circle *c) {
    int i;
    for(i = 0; i <= 100; i++) {
        c->xInd[i] = c->center->xPos + c->radius * cos(i * (2*3.14159/100));
        c->yInd[i] = c->center->yPos + c->radius * sin(i * (2*3.14159/100));
    }
}

bool hasCircle(Primitive *tail) {
    return tail->prev->shape == CIRCLE;
}

bool hasRect(Primitive *tail) {
    return tail->prev->shape == RECT;
}

bool hasTriangle(Primitive *tail) {
    return tail->prev->shape == TRIANGLE;
}

bool hasLine(Primitive *tail) {
    return tail->prev->shape == LINE;
}

void cursorLine() {
    if(mouseDown){
        if(!hasLine(tail)) {
//          printf(mouseX);
//          printf(mouseY);
            Pt *startPt = (Pt *)tail->prev->data;
            Pt *endPt = makePt(mouseX, mouseY, colors);
            Line *line = makeLine(startPt, endPt, colors);
            Primitive *primPt = makePrim(POINT, endPt);
            Primitive *primLine = makePrim(LINE, line);
            addPrim(tail, primPt);
            addPrim(tail, primLine);
        }
        else {
            glLineStipple(1, 0x00FF);
            ((Pt *)(tail->prev->prev->data))->xPos = mouseX;
            ((Pt *)(tail->prev->prev->data))->yPos = mouseY;
        }
    }
}

void cursorTriangle(){
    if(!hasTriangle(tail)){
        cursorLine();
    }
    else {
        if(mouseDown){
            ((Pt *)(tail->prev->prev->data))->xPos = mouseX;
            ((Pt *)(tail->prev->prev->data))->yPos = mouseY;
        }
    }
}

void cursorRect() {
    if(mouseDown) {
        if(!hasRect(tail)) {
            Pt *startPt = (Pt *)tail->prev->data;
            Pt *endPt = makePt(mouseX, mouseY, colors);
            Rect *rect = makeRect(startPt, endPt, colors);
            Primitive *addPt = makePrim(POINT, endPt);
            Primitive *addRect = makePrim(RECT, rect);
            addPrim(tail, addPt);
            addPrim(tail, addRect);
        }
        else {
            ((Pt *)(tail->prev->prev->data))->xPos = mouseX;
            ((Pt *)(tail->prev->prev->data))->yPos = mouseY;
            ((Rect *)(tail->prev->data))->v2->xPos = mouseX;
            ((Rect *)(tail->prev->data))->v4->yPos = mouseY;
        } //else
    } //if mouseDown
}

void cursorCircle() {
    if(mouseDown) {
        if(!hasCircle(tail)) {
            Pt *startPt = (Pt *)tail->prev->data;
            Pt *endPt = makePt(mouseX, mouseY, colors);
            Circle *circle = makeCircle(startPt, endPt, colors);
            Primitive *endPtPart = makePrim(POINT, endPt);
            Primitive *circPart = makePrim(CIRCLE, circle);
            addPrim(tail, endPtPart);
            addPrim(tail, circPart);
        } //!hasCircle
        else {
            ((Pt *)(tail->prev->prev->data))->xPos = mouseX;
            ((Pt *)(tail->prev->prev->data))->yPos = mouseY;
            Circle *circle = (Circle *) tail->prev->data;
            circle->center->xPos = (circle->v1->xPos + mouseX)/2;
            circle->center->yPos = (circle->v1->yPos + mouseY)/2;
            circle->radius = circRadius(circle->center, circle->v1);
            checkCircleIndices(circle);
        } //else
    } //if(mouseDown)
}

void switchCol(GLfloat *target, GLfloat *colorClicked) {

        memcpy(target, colorClicked, 3*sizeof(GLfloat));
}

void freePrim(Primitive* prim) {
    prim->prev = NULL;
    prim->next = NULL;
    if(prim->shape == POINT) {
        free((Pt *) prim->data);
    }
    else if(prim->shape == LINE) {
        free((Line *) prim->data);
    }
    else if(prim->shape == TRIANGLE) {
        free((Triangle *) prim->data);
    }
    else if(prim->shape == RECT) {
        free((Rect *) prim->data);
    }
    else if(prim->shape == CIRCLE) {
        free((Circle *) prim->data);
    }
}

void removePrev(Primitive *prim) {
    Primitive *curr = prim->prev;
    prim->prev = curr->prev;
    freePrim(curr);
}

void clickedPt(){
    pressedCoords = makePt(mouseX, mouseY, colors);
    Primitive *retPt = makePrim(POINT,pressedCoords);
    addPrim(tail, retPt);
}

void mouseDownCircle() {
    clickedPt();
    if(circleFree(tail)){
        Pt *pt1 = tail->prev->prev->data;
        Pt *pt2 = tail->prev->data;
        Circle *circleMade = makeCircle(pt1, pt2, colors);
        Primitive *retCirc = makePrim(CIRCLE, circleMade);
        addPrim(tail, retCirc);
    } //circleFree
}

void mouseDownLine() {
    clickedPt();
    if(lineFree(tail)) {
        Pt *p1 = tail->prev->data;
        Pt *p2 = tail->prev->prev->data;
        Line *linePrePrim = makeLine(p1, p2, colors);
        Primitive *linePrim = makePrim(LINE, linePrePrim);
        addPrim(tail, linePrim);
    }
}

void mouseDownRect() {
    clickedPt();
    if(rectFree(tail)) {
        Pt *p1 = tail->prev->prev->data;
        Pt *p2 = tail->prev->data;
        Rect *rectPre = makeRect(p1, p2, colors);
        Primitive *rectMade = makePrim(rectPre, makeRect);
        addPrim(tail, rectMade);
    }
}

void mouseDownTri() {
    if(!triangleFree(tail)){
        mouseDownLine();
    } //if
    else {
        if(tail->prev->shape == LINE){
            removePrev(tail);
        } //line
        clickedPt();
        Pt* v1 = (Pt *)tail->prev->data;
        Pt* v2 = (Pt *)tail->prev->prev->data;
        Pt* v3 = (Pt *)tail->prev->prev->prev->data;
        Triangle* tri = makeTriangle(v1, v2, v3, colors);
        Primitive *triMade = makePrim(TRIANGLE, tri);
        addPrim(tail, triMade);
    }
}

void mouseUpTri() {
    if(hasTriangle(tail)){
        Triangle *tri = (Triangle *)tail->prev->data;
        tri->col = (GLfloat *) malloc(sizeof(GLfloat) * 3);
        memcpy(tri->col, colors, sizeof(GLfloat) * 3);
        tri->dashed = false;
        tri->fill = filled;
        ((Pt *) tail->prev->prev->data)->show = false;
        ((Pt *) tail->prev->prev->prev->data)->show = false;
        ((Pt *) tail->prev->prev->prev->prev->data)->show = false;
    }
}
void mouseUpLine() {
    if(hasLine(tail)){
        Line *line = (Line *)tail->prev->data;
        line->col = (GLfloat *) malloc(sizeof(GLfloat) * 3);
        memcpy(line->col, colors, sizeof(GLfloat) * 3);
        line->dashed = false;
        line->fill = filled;
        ((Pt *)(tail->prev->prev->data))->show = false;
        ((Pt *)(tail->prev->prev->prev->data))->show = false;
    } //hasline
}

void mouseUpRect() {
    if(hasRect(tail)) {
        Rect *rect = (Rect *) tail->prev->data;
        rect->col = (GLfloat *) malloc(3*sizeof(GLfloat));
        memcpy(rect->col, colors, 3*sizeof(GLfloat));
        rect->dashed = false;
        rect->fill = filled;
        ((Pt *) tail->prev->prev->prev->data)->show = false;
        ((Pt *) tail->prev->prev->data)->show = false;
    } //hasRect
}

void mouseUpCircle() {
    if(hasCircle(tail)) {
        Circle* c = (Circle *) tail->prev->data;
        c->col = (GLfloat *) malloc(3*sizeof(GLfloat));
        memcpy(c->col, colors, 3*sizeof(GLfloat));
        c->dashed = false;
        c->fill = filled;
        ((Pt *) tail->prev->prev->prev->data)->show = false;
        ((Pt *) tail->prev->prev->data)->show = false;
    } //hasCircle
}

void reshape(GLFWwindow *wind, float w, float h) {
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.0, w, h, 0.0);
//    glMatrixMode(GL_MODELVIEW);
    lolWidth = w;
    lolHeight = h;
}


Polygon *makePolygon(double x) {
    Polygon *new1;
    if ((new1 = (Polygon *) malloc( sizeof(Polygon)))!= 0) {
        new1->data =  &x;
    }
    else {
        exit(0);
    }
    return new1;
}


void drawPt(Pt *p) {
    if(p->show) {
        glColor3fv(p->col);
        glBegin(GL_POINTS);
        glVertex2f(p->xPos, p->yPos);
        glEnd();
    }
}

void drawLine(Line *l) { //(double x1, double y1, double x2, double y2) {
    if(l->dashed) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
    }
    else {
        glDisable(GL_LINE_STIPPLE);
    }
    glColor3fv(l->col);
    glBegin(GL_LINES);
    glVertex2f(l->p1->xPos, l->p1->yPos);
    glVertex2f(l->p2->xPos, l->p2->yPos);
    glEnd();
}


void drawRectangle(Rect* r) {
    if(r->dashed) {
        glEnable(GL_LINE_STIPPLE);
    }
    else {
        glDisable(GL_LINE_STIPPLE);
    }
    if(r->fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glColor3fv(r->col);
    glBegin(GL_QUADS);
    glVertex2f(r->v1->xPos, r->v1->yPos);
    glVertex2f(r->v2->xPos, r->v2->yPos);
    glVertex2f(r->v3->xPos, r->v3->yPos);
    glVertex2f(r->v4->xPos, r->v4->yPos);
    glEnd();
}

void drawTriangle(Triangle *t) {
    if(t->dashed) {
        glEnable(GL_LINE_STIPPLE);
    }
    else {
        glDisable(GL_LINE_STIPPLE);
    }
    if(t->fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glColor3fv(t->col);
    glBegin(GL_TRIANGLES);
    glVertex2f(t->v1->xPos, t->v1->yPos);
    glVertex2f(t->v2->xPos, t->v2->yPos);
    glVertex2f(t->v3->xPos, t->v3->yPos);
    glEnd();
}

void drawCircle(Circle *c) {
    if(c->dashed) {
        glEnable(GL_LINE_STIPPLE);
    }
    else {
        glDisable(GL_LINE_STIPPLE);
    }
    glColor3fv(c->col);
    if(c->fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c->center->xPos, c->center->yPos);
    }
    else {
        glBegin(GL_LINE_LOOP);
    }
    GLint i;
    for(i = 0; i <= 100; i++) {
        glVertex2f(c->xInd[i], c->yInd[i]);
    }
    glEnd();
}

void draw(Primitive *p1) {
    if(p1->shape == POINT) {
        drawPt((Pt *) p1->data);
    }
    else if(p1->shape == LINE) {
        drawLine((Line *) p1->data);
    }
    else if(p1->shape == TRIANGLE) {
        drawTriangle((Triangle *) p1->data);
    }

    else if(p1->shape == RECT) {
        drawRectangle((Rect *) p1->data);
    }
    else if(p1->shape == CIRCLE) {
        drawCircle((Circle *) p1->data);
    }
}

void drawAll(Primitive *p2) {
    Primitive *curr = p2->next;
    while (curr->shape != -1) {
        draw(curr);
        curr = curr->next;
    }
}


void key_callback(GLFWwindow *w, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) {
        switch(key) {
            case GLFW_KEY_ESCAPE :
            case 'q' :
            case 'Q':
                clearAll(head);
                glfwSetWindowShouldClose(w, GL_TRUE);
                break;
            case 'f':
            case 'F':
                filled = !filled;
                break;
            case 'r':
            case 'R':
                if(blackColor || greenColor || blueColor) {
                    memcpy(colors, red, 3*sizeof(GLfloat));
                    greenColor = false;
                    blackColor = false;
                    redColor = true;
                    blueColor = false;
                }
                else {
                    memcpy(colors, black, 3*sizeof(GLfloat));
                    greenColor = false;
                    blackColor = true;
                    redColor = false;
                    blueColor = false;
                }

                break;
            case 'g':
            case 'G':
                if(blackColor || blueColor || redColor) {
                    memcpy(colors, green, 3*sizeof(GLfloat));
                    greenColor = true;
                    blackColor = false;
                    redColor = false;
                    blueColor = false;
                }
                else {
                    memcpy(colors, black, 3*sizeof(GLfloat));
                    greenColor = false;
                    blackColor = true;
                    redColor = false;
                    blueColor = false;
                }
                break;
            case 'b':
            case 'B':
                if(blackColor || greenColor || redColor) {
                    memcpy(colors, blue, 3*sizeof(GLfloat));
                    greenColor = false;
                    blackColor = false;
                    redColor = false;
                    blueColor = true;
                }
                else {
                    memcpy(colors, black, 3*sizeof(GLfloat));
                    greenColor = false;
                    blackColor = true;
                    redColor = false;
                    blueColor = false;
                }
                break;
            case 'c':
            case 'C':
                clearAll(head);
                break;
            case GLFW_KEY_F1:
                lineMode = true;
                circleMode = false;
                rectangleMode = false;
                triangleMode = false;
                break;
            case GLFW_KEY_F2:
                lineMode = false;
                circleMode = false;
                rectangleMode = false;
                triangleMode = true;
                break;
            case GLFW_KEY_F3:
                lineMode = false;
                circleMode = false;
                rectangleMode = true;
                triangleMode = false;
                break;
            case GLFW_KEY_F4:
                lineMode = false;
                circleMode = true;
                rectangleMode = false;
                triangleMode = false;
                break;
//          case GLFW_KEY_F5:
            default:
                break;
        }
    }
}


void mouse(GLFWwindow* window, int button, int action, int mods) {
    GLdouble x, y;
    glfwGetCursorPos(window, &x, &y);
    switch(button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouseY = -(2*(y/lolHeight) - 1);
            mouseX = 2*(x/lolWidth) - 1;
            if(action == GLFW_PRESS) {
                mouseDown = true;
                if(lineMode) {
                    mouseDownLine();
                }
                else if(triangleMode) {
                    mouseDownTri();
                }
                else if(rectangleMode) {
                    mouseDownRect();
                }
                else if(circleMode) {
                    mouseDownCircle();
                }
            } //glfw_press
            else {
                mouseDown = false;
                if(lineMode) {
                    mouseUpLine();
                }
                else if(triangleMode) {
                    mouseUpTri();
                }
                else if(rectangleMode) {
                    mouseUpRect();
                }
                else if(circleMode) {
                    mouseUpCircle();
                }
            } //else
    } //case
}

void cursor(GLFWwindow* window, double xpos, double ypos) {
    mouseX = 2*(xpos/WIDTH) - 1;
    mouseY = -1*(2*(ypos/HEIGHT) - 1);
    //if(xpos > 0 && xpos < width && ypos > 0 && ypos < height) { //same as entry, essentially
        if(lineMode) {
            cursorLine();
        }
        else if(triangleMode) {
            cursorTriangle();
        }
        else if(rectangleMode) {
            cursorRect();
        }
        else if(circleMode) {
            cursorCircle();
        }
}

int main() {
    GLFWwindow* window;
    head = initLinkedList();
    tail = head->next;
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    window = glfwCreateWindow(WIDTH, HEIGHT, "HW 1", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowPos(window, 100, 100);
    
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);
    
    //glfwSetWindowSizeCallback
    glfwSetFramebufferSizeCallback(window, framebuffer_resize);

    glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorEnterCallback(window, cursor);
    glfwSetCursorPosCallback(window, cursor);

    glfwSetMouseButtonCallback(window, mouse);

    glfwSetCursorPosCallback(window, cursor);

    init();
    while (!glfwWindowShouldClose(window)) { //loop till user closes window
        glClear(GL_COLOR_BUFFER_BIT); //clear to background color
        drawAll(head);
        glfwSwapBuffers(window); //Swap front and back buffers
        // Poll for and process events
        glfwPollEvents();
    }
    
    glfwTerminate();
    freePrim(head);
    freePrim(tail);
    exit(EXIT_SUCCESS); //glfwDestroyWindow(window);
}
