#ifndef POINT_H
#define POINT_H


//struct point *list;
//struct point table[100];

struct point {
    int x;
    int y;
    struct point *next;
};

struct point *list;
struct point table[100];

#endif
