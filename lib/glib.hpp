#ifndef GLIB_HPP
#define GLIB_HPP

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <math.h>

#define PI 3.14159265f

using namespace std;
// Classes.

// Defintion of an edge of our polygon. Contains useful coordinates for the scanline.
class Edges {
public:
    GLint minY;
    GLint maxY;
    GLint minX;
    GLint maxX;
    GLfloat xVal; // X value at the minimum vertex, this is where we start our scanline on the x-axis.
    GLfloat slope;
    // Tuples are (x, y).
    Edges(tuple<GLint, GLint> vertexOne, tuple<GLint, GLint> vertexTwo);
};


// Structs.

// Sort Edges by minY values and, if the same, then sort by minX values.
struct less_than_key
{
    inline bool operator() (const Edges& struct1, const Edges& struct2)
    {
        if (struct1.minY != struct2.minY)
            return (struct1.minY < struct2.minY);
        return (struct1.minX < struct2.minX);
    }
};

// Sorts Edges by current x values.
struct by_x_val_key
{
    inline bool operator() (const Edges& struct1, const Edges& struct2)
    {
        return (struct1.xVal < struct2.xVal);
    }
};

// Stores color data for a single pixel.
struct RGBType {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

// Globals
extern int X_DOWN;
extern GLint scanline;
extern bool TOGGLE_STATE;
extern bool MOUSE_STATE;
extern bool DRAWING; // Enables or disables user input.
extern bool EDGE_PARITY;
extern vector<tuple<GLint, GLint>> points; // Contains all the vertexes.
extern vector<Edges> allEdges; // Contains every Edge.
extern vector<Edges> activeEdges; // Contains Edges currently intersecting the scanline.
extern RGBType *pixels; // Contains every pixel on the screen.
extern int *wH, *wW;

void drawLine(float X1, float Y1, float X2, float Y2);
void drawCircle(int x_centre, int y_centre, int radius);
void addEdge(int x1, int y1, int x2, int y2);
void scanFill();
void init();
void fillPolygon();

#endif