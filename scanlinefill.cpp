#include <windows.h>

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

// Calculates neccessary values from two vertexes.
Edges::Edges(tuple<GLint, GLint> vertexOne, tuple<GLint, GLint> vertexTwo) {
    this->minY = min(get<1>(vertexOne), get<1>(vertexTwo));
    this->maxY = max(get<1>(vertexOne), get<1>(vertexTwo));
    this->minX = min(get<0>(vertexOne), get<0>(vertexTwo));
    this->maxX = max(get<0>(vertexOne), get<0>(vertexTwo));


    if (min(get<1>(vertexOne), get<1>(vertexTwo)) == get<1>(vertexOne)) {
        this->xVal = get<0>(vertexOne);
    } else {
        this->xVal = get<0>(vertexTwo);
    }

    // Calculates scope, casting for float division.
    this->slope =
    static_cast<GLfloat>(static_cast<GLfloat>(get<1>(vertexOne) - get<1>(vertexTwo)))
    / static_cast<GLfloat>((get<0>(vertexOne) - get<0>(vertexTwo)));
}

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
int X_DOWN;
GLint scanline;
bool TOGGLE_STATE;
bool MOUSE_STATE;
bool DRAWING; // Enables or disables user input.
bool EDGE_PARITY;
vector<tuple<GLint, GLint>> points; // Contains all the vertexes.
vector<Edges> allEdges; // Contains every Edge.
vector<Edges> activeEdges; // Contains Edges currently intersecting the scanline.
RGBType *pixels; // Contains every pixel on the screen.
int *wH, *wW;

// Draws pixels from (x1, scanline) to (x2, scanline).
void drawPixels(GLfloat x1, GLfloat x2) {
    // round points, not floor, as we want the x values to change with the scanline.
    int i1 = roundf(x1);
    int i2 = roundf(x2);
    int count = 0;

    for (int i = ((*wW * (scanline)) + i1); i < ((*wW * (scanline)) + i2); i++, count++) {
        pixels[i].r = 1;
        pixels[i].b = 1;
        pixels[i].g = 1;
    //    glutPostRedisplay();
   //     printf("change pixel %d, %d\n", (i1 + count), scanline);
    }
  //  printf("Pixels drawn.\n");
}

// Removes edges from the Active Edges if the maxY of the edge is the same as the scanline.
void removeActiveEdgesByScanline() {
    for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); ) {
        if (it->maxY == scanline) {
            activeEdges.erase(it);
        } else {
            it++;
        }
    }
}

// Updates the x values of the Active Edges for the next scanline.
void updateXValues() {
    for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); it++) {
        it->xVal += (1/it->slope);
    }
}

// Adds new Active Edges from All Edges if the scanline reaches their minimum Y value.
void updateActiveEdges() {
    for (vector<Edges>::iterator it = allEdges.begin(); it < allEdges.end(); it++) {
        if (it->minY == scanline) {
            activeEdges.push_back(*it);
        }
        if (it->minY > scanline) {
            return;
        }
    }
}

// Set scanline value to the smallest Y value
void initScanline() {
    if (allEdges.size() != 0) {
        scanline = allEdges.at(0).minY;
      //  glutPostRedisplay();
    }
}

// Sort Edges to know which order the scanline hits them.
void sortAndFilterEdges() {
    sort(allEdges.begin(), allEdges.end(), less_than_key());
    for (vector<Edges>::iterator it = allEdges.begin(); it < allEdges.end(); it++) {
        // Don't need to deal with horizontal lines, as that's the direction we're scanning.
        if (it->slope == 0) {
            allEdges.erase(it);
        }
    }
}

void sortActiveEdgesByXValues() {
    sort(activeEdges.begin(), activeEdges.end(), by_x_val_key());
}

// Actually fills the polygon, finds the intersections of the edges and the scanline, draws, and updates
// the Edges.
void fillPolygon() {
    while (activeEdges.size() != 0) {
        for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); it++) {
    //        printf("drawing from %f to %f\n", it->xVal, (it+1)->xVal);
            drawPixels(it->xVal, (it+1)->xVal);
            it++;
        }
        scanline++;
        removeActiveEdgesByScanline();
        updateXValues();
        updateActiveEdges();
        sortActiveEdgesByXValues();
     //   glutPostRedisplay();
    }
}

void init(void) {

    //Initialize RGB array
    delete pixels;
    pixels = new RGBType[*wH * *wW];

    // Set everything to white initially.
    for (int i = 0; i < *wH * *wW; i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
    }

    allEdges.clear();
    activeEdges.clear();
}

// Right click to complete polygon and kick off filling process.
/* void menu(int id) {
    switch (id) {
        case 1:
            Edges newEdge(points.at(0), points.at(points.size()-1));
            allEdges.push_back(newEdge);
            sortAndFilterEdges();
            initScanline();
            updateActiveEdges();
            DRAWING = true;
            glutPostRedisplay();
            fillPolygon();
            break;
    }
    glutPostRedisplay();
} */

void scanFill() {
    sortAndFilterEdges();
    initScanline();
    updateActiveEdges();
    fillPolygon();
}

void addEdge(int x1, int y1, int x2, int y2) {
    points.push_back(tuple<GLint, GLint>(x1, y1));
    points.push_back(tuple<GLint, GLint>(x2, y2));
    Edges newEdge(points.at(points.size()-2), points.at(points.size()-1));
    allEdges.push_back(newEdge);
}

// Stores vertexes on click.
/* void mouse_down(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN && !DRAWING) {
                printf("ADDING POINT %d and %d", x, 500 - y);
                points.push_back(tuple<GLint, GLint>(x, y));
                if (points.size() > 1) {
                    Edges newEdge(points.at(points.size()-2), points.at(points.size()-1));
                    allEdges.push_back(newEdge);
                }
                glutPostRedisplay();
            }
    }
    glutPostRedisplay();
} */
