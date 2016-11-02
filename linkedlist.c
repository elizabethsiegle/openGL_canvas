#include "linkedlist.h"
#include "canvas.h"

Node* initLinkedList() {
    Node* head = makeNode(-1, 0);
    Node* tail = makeNode(-1, 0);
    head->next = tail;
    tail->prev = head;
    return head;
}

void clearAll(Node *head) {
    while(head->next->shape != -1) {
        Node* curr = head->next;
        head->next = head->next->next;
        freeNode(curr);
    }
    head->next->prev = head;
}

Pt *makePt(GLfloat x, GLfloat y, GLfloat colors[]) {
    Pt *p;
    if( (p = (Pt *) malloc(sizeof(Pt))) != NULL) {
        p->show = true;
        p->xPos = x;
        p->yPos = y;
        p->col = colors;
    }
    return p;
}

bool lineFree(Node *t) {
    return t->prev->shape == POINT &&
    ((Pt *)(t->prev->data))->show &&
    t->prev->prev->shape == POINT &&
    ((Pt *)(t->prev->prev->data))->show;
}

bool triangleFree(Node *tail) {
    return tail->prev->shape == LINE && ((Line *)(tail->prev->data))->dashed == true;
}

bool rectFree(Node* tail) {
    return tail->prev->shape == POINT &&
    ((Pt *)(tail->prev->data))->show &&
    tail->prev->prev->shape == POINT &&
    ((Pt *)(tail->prev->prev->data))->show;
}

bool circleFree(Node* tail) {
    return tail->prev->shape == POINT &&
    ((Pt *)(tail->prev->data))->show &&
    tail->prev->prev->shape == POINT &&
    ((Pt *)(tail->prev->prev->data))->show;
}

void addNode(Node *tail, Node *newNode) {
    newNode->next = tail;
    newNode->prev = tail->prev;
    tail->prev->next = newNode;
    tail->prev = newNode;
}

Line *makeLine(Pt *xPos, Pt *yPos, GLfloat colors[]) {
    Line * line;
    if((line = (Line *) malloc(sizeof(Line))) != NULL) {
        line->p1 = xPos;
        line->p2 = yPos;
        line->col = colors;
        line->dashed = true;
        line->fill = false;
    }
    else {
        exit(0);
    }
    return line;
}

Triangle *makeTriangle(Pt* p1, Pt* p2, Pt* p3, GLfloat colors[]) {
    Triangle* tri;
    if((tri = (Triangle *) malloc(sizeof(Triangle))) != NULL) {
        if(p1->xPos < p2->xPos){
            tri->v1 = p1;
            tri->v2 = p2;
        }
        else {
            tri->v1 = p2;
            tri->v2 = p1;
        }
        GLfloat slope = (tri->v1->yPos - tri->v2->yPos) / (tri->v1->xPos - tri->v2->xPos);
        if(slope * (p3->xPos) <= p3->yPos){
            tri->v3 = p3;
        } else {
            Pt *temp = tri->v2;
            tri->v2 = p3;
            tri->v3 = temp;
        }
        tri->col = colors;
        tri->dashed = true;
        tri->fill = false;
    }
    else {
        exit(0);
    }
    return tri;
}

Rect *makeRect(Pt* p1, Pt* p2, GLfloat colors[]) {
    Rect* rect;
    if((rect = (Rect *) malloc(sizeof(Rect))) != NULL) {
        rect->v1 = p1;
        rect->v2 = makePt(p2->xPos, p1->yPos, colors);
        rect->v3 = p2;
        rect->v4 = makePt(p1->xPos, p2->yPos, colors);
        rect->col = colors;
        rect->dashed = true;
        rect->fill = false;
    }
    else {
        exit(0);
    }
    return rect;
}

Circle *makeCircle(Pt *d1, Pt *d2, GLfloat colors[]) {
    Circle *c;
    if((c = (Circle *) malloc(sizeof(Circle))) != NULL) {
        c->v1 = d1;
        c->v2 = d2;
        c->center = makePt((d1->xPos + d2->xPos)/2, (d1->yPos + d2->yPos)/2, colors);
        c->radius = circRadius(c->center, c->v1);
        c->xInd = (GLfloat *) malloc(sizeof(GLfloat) * (101));
        c->yInd = (GLfloat *) malloc(sizeof(GLfloat) * (101));
        checkCircleIndices(c);
        c->col = colors;
        c->dashed = true;
        c->fill = false;
    }
    else {
        exit(0);
    }
    return c;
}

Node *makeNode(int shape, void *data) {
    Node* n;
    if((n = (Node *) malloc(sizeof(Node))) != NULL) {
        n->shape = shape;
        n->data = data;
    }
    else {
        exit(0);
    }
    return n;
}
