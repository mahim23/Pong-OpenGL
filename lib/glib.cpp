#include "glib.hpp"
using namespace std;

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

// Scanline helper functions

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
    }
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
    delete pixels; //free memory to prevent leaks
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

void drawLine(float X1, float Y1, float X2, float Y2) {
    float p[2], t = 0;

    if ((X1 == X2) || (Y2 - Y1) / (X2 - X1) > 1) {
        float temp1 = X1;
        float temp2 = X2;
        X1 = Y1;
        X2 = Y2;
        Y1 = temp1;
        Y2 = temp2;
        t = 1;
    }

    glBegin(GL_POINTS);

    glColor3f(1.0f, 1.0f, 1.0f);

    int m_new = 2 * (Y2 - Y1);
    int slope_error_new = m_new - (X2 - X1);
    for (int X = X1, Y = Y1; X <= X2; X++)
    {
        p[0] = X;
        p[1] = Y;
        if (t == 1) {
            float temp = p[0];
            p[0] = p[1];
            p[1] = temp;
        }
        glVertex2fv(p);

        slope_error_new += m_new;

        if (slope_error_new >= 0)
        {
            Y++;
            slope_error_new  -= 2 * (X2 - X1);
        }
   }
    glEnd();
}

void drawCircle(int x_centre, int y_centre, int radius) {

    glColor3f(1.0f, 1.0f, 1.0f);
    int x = radius, y = 0;
    glBegin(GL_POINTS);
    glVertex2d(x + x_centre, y + y_centre);
    if (radius > 0){
        glVertex2d(x + x_centre, -y + y_centre);
        glVertex2d(y + x_centre, x + y_centre);
        glVertex2d(-y + x_centre, x + y_centre);
    }
    glEnd();
    drawLine(-x + x_centre, y_centre, x + x_centre, y_centre);
    int P = 1 - radius;
    while (x > y){
        y++;
        if (P <= 0)
            P = P + 2*y + 1;
        else{
            x--;
            P = P + 2*y - 2*x + 1;
        }
        if (x < y)
            break;

        glBegin(GL_POINTS);
            glVertex2d(x + x_centre, y + y_centre);
            glVertex2d(-x + x_centre, y + y_centre);
            glVertex2d(x + x_centre, -y + y_centre);
            glVertex2d(-x + x_centre, -y + y_centre);
        glEnd();
        drawLine(-x + x_centre, y + y_centre, x + x_centre, y + y_centre);
        drawLine(-x + x_centre, -y + y_centre, x + x_centre, -y + y_centre);
        if (x != y){
            glBegin(GL_POINTS);
                glVertex2d(y + x_centre, x + y_centre);
                glVertex2d(-y + x_centre, x + y_centre);
                glVertex2d(y + x_centre, -x + y_centre);
                glVertex2d(-y + x_centre, -x + y_centre);
            glEnd();
            drawLine(-y + x_centre, x + y_centre, y + x_centre, x + y_centre);
            drawLine(-y + x_centre, -x + y_centre, y + x_centre, -x + y_centre);
        }
    }
}