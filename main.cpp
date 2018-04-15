// # include <windows.h>  // for MS Windows
#include <iostream>
#include "lib/game.hpp"
using namespace std;

// Global variables
char title[] = "Pong (2D)"; // Windowed mode's title
int windowWidth = 480;      // Windowed mode's width
int windowHeight = 640;     // Windowed mode's height
int windowPosX = 50;        // Windowed mode's top-left corner x
int windowPosY = 50;        // Windowed mode's top-left corner y

int player1 = 0, player2 = 0;
GLfloat ballRadius = 5;          // Radius of the bouncing ball
GLfloat ballX = windowWidth / 2; // Ball's center (x, y) position
GLfloat ballY = windowHeight / 2;
int paddle1X = windowWidth / 2;
int paddle1Y = windowHeight - 10;
int paddle2X = windowWidth / 2;
int paddle2Y = 10;
GLfloat paddleLength = 80;
GLfloat paddleBreadth = 20;
GLfloat ballXMax, ballXMin, ballYMax, ballYMin; // Ball's center (x, y) bounds
GLfloat xSpeed = 4;                             // Ball's speed in x and y directions
GLfloat ySpeed = 6;
int paddleSpeed = 5;
int refreshMillis = 30; // Refresh period in milliseconds
int movement[4] = {0, 0, 0, 0};
int flag = 1;

int a = 20, b = 60, c = 30, d = 40;

// Projection clipping area
GLdouble clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop;

/* Initialize OpenGL Graphics */
void initGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set background (clear) color to black
    wH = &windowHeight;
    wW = &windowWidth;
}

/* Callback handler for window re-paint event */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    glMatrixMode(GL_MODELVIEW);   // To operate on the model-view matrix
    glLoadIdentity();             // Reset model-view matrix

    glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_FLOAT, pixels);
    drawCircle(ballX, ballY, ballRadius);
    drawPaddle(paddleLength, paddleBreadth, paddle1X, paddle1Y);
    drawPaddle(paddleLength, paddleBreadth, paddle2X, paddle2Y);
    if (flag)
    {
        init();
        fillPaddles(paddleLength, paddleBreadth, paddle1X, paddle1Y);
        fillPaddles(paddleLength, paddleBreadth, paddle2X, paddle2Y);
        flag = 0;
    }

    glutSwapBuffers(); // Swap front and back buffers (of double buffered mode)

    // Animation Control - compute the location for the next refresh
    ballX += xSpeed;
    ballY += ySpeed;
    // Check if the ball exceeds the edges
    if (ballX > ballXMax)
    {
        ballX = ballXMax;
        xSpeed = -xSpeed;
    }
    else if (ballX < ballXMin)
    {
        ballX = ballXMin;
        xSpeed = -xSpeed;
    }

    if ((ballY - (windowHeight - paddleBreadth - ballRadius) < ySpeed) &&
        (ballY - (windowHeight - paddleBreadth - ballRadius) >= 0) &&
        ySpeed > 0 &&
        (ballX < paddle1X + paddleLength / 2 + ballRadius) &&
        (ballX > paddle1X - paddleLength / 2 - ballRadius))
    {

        ySpeed = -ySpeed;
    }
    else if ((ballY - (paddleBreadth + ballRadius) > ySpeed) &&
             (ballY - (paddleBreadth + ballRadius) <= 0) &&
             ySpeed < 0 &&
             (ballX < paddle2X + paddleLength / 2 + ballRadius) &&
             (ballX > paddle2X - paddleLength / 2 - ballRadius))
    {

        ySpeed = -ySpeed;
    }
    else if (ballY > ballYMax)
    {
        player2++;
        ballX = windowWidth / 2;
        ballY = windowHeight / 2;
        ySpeed = -ySpeed;
        cout << player1 << "\t-\t" << player2 << endl;
        // printf("%d - %d\n", player1, player2);
    }
    else if (ballY < ballYMin)
    {
        player1++;
        ballX = windowWidth / 2;
        ballY = windowHeight / 2;
        ySpeed = -ySpeed;
        cout << player1 << "\t-\t" << player2 << endl;
        // printf("%d - %d\n", player1, player2);
    }

    if (movement[0] && !movement[1] && paddle1X - paddleLength / 2 >= paddleSpeed)
    {
        paddle1X -= paddleSpeed;
    }
    else if (!movement[0] && movement[1] && paddle1X + paddleLength / 2 <= windowWidth - paddleSpeed)
    {
        paddle1X += paddleSpeed;
    }
    if (movement[2] && !movement[3] && paddle2X - paddleLength / 2 >= paddleSpeed)
    {
        paddle2X -= paddleSpeed;
    }
    else if (!movement[2] && movement[3] && paddle2X + paddleLength / 2 <= windowWidth - paddleSpeed)
    {
        paddle2X += paddleSpeed;
    }
    if (movement[0] || movement[1] || movement[2] || movement[3])
    {
        init();
        fillPaddles(paddleLength, paddleBreadth, paddle1X, paddle1Y);
        fillPaddles(paddleLength, paddleBreadth, paddle2X, paddle2Y);
    }
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        movement[3] = 1;
        break;
    case GLUT_KEY_LEFT:
        movement[2] = 1;
        break;
    }
}

void specialUpKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        movement[3] = 0;
        break;
    case GLUT_KEY_LEFT:
        movement[2] = 0;
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 97:
        movement[0] = 1;
        break;
    case 100:
        movement[1] = 1;
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 97:
        movement[0] = 0;
        break;
    case 100:
        movement[1] = 0;
        break;
    }
}

/* Call back when the windows is re-sized */
void reshape(GLsizei width, GLsizei height)
{

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset the projection matrix

    gluOrtho2D(0, width, 0, height);
    ballXMin = ballRadius;
    ballXMax = width - ballRadius;
    ballYMin = ballRadius;
    ballYMax = height - ballRadius;
    paddle1X = (float)paddle1X / windowWidth * width;
    paddle2X = (float)paddle2X / windowWidth * width;
    ballX = ballX / windowWidth * width;
    ballY = ballY / windowHeight * height;
    paddleLength *= (float)width / windowWidth;
    paddleBreadth *= (float)height / windowHeight;
    ballRadius *= (float)height / windowHeight;
    windowHeight = height;
    windowWidth = width;
    paddle1Y = height - paddleBreadth / 2;
    init();
    fillPaddles(paddleLength, paddleBreadth, paddle1X, paddle1Y);
    fillPaddles(paddleLength, paddleBreadth, paddle2X, paddle2Y);
    //  glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_FLOAT, pixels);
}

/* Called back when the timer expired */
void Timer(int value)
{
    glutPostRedisplay();                    // Post a paint request to activate display()
    glutTimerFunc(refreshMillis, Timer, 0); // subsequent timer call at milliseconds
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char **argv)
{
    cout<<"Welcome to the classic game of Pong"<<endl;
    cout<<"Player 1 - Player 2"<<endl;
    glutInit(&argc, argv);                          // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);               // Enable double buffered mode
    glutInitWindowSize(windowWidth, windowHeight);  // Initial window width and height
    glutInitWindowPosition(windowPosX, windowPosY); // Initial window top-left corner (x, y)
    glutCreateWindow(title);                        // Create window with given title
    glutDisplayFunc(display);                       // Register callback handler for window re-paint
    glutReshapeFunc(reshape);                       // Register callback handler for window re-shape
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutSpecialUpFunc(specialUpKeys);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(0, Timer, 0); // First timer call immediately
    initGL();                   // Our own OpenGL initialization
    glutMainLoop();             // Enter event-processing loop
    return 0;
}
