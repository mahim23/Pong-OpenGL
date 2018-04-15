#include "game.hpp"

void drawPaddle(int length, int breadth, int centreX, int centreY) {
    drawLine(centreX - length/2, centreY + breadth/2, centreX + length/2, centreY + breadth/2);
    drawLine(centreX + length/2, centreY - breadth/2, centreX + length/2, centreY + breadth/2);
    drawLine(centreX - length/2, centreY - breadth/2, centreX + length/2, centreY - breadth/2);
    drawLine(centreX - length/2, centreY - breadth/2, centreX - length/2, centreY + breadth/2);
}

void fillPaddles(int length, int breadth, int centreX, int centreY) {
    addEdge(centreX - length/2, centreY + breadth/2, centreX + length/2, centreY + breadth/2);
    addEdge(centreX + length/2, centreY - breadth/2, centreX + length/2, centreY + breadth/2);
    addEdge(centreX - length/2, centreY - breadth/2, centreX + length/2, centreY - breadth/2);
    addEdge(centreX - length/2, centreY - breadth/2, centreX - length/2, centreY + breadth/2);
    scanFill();
}