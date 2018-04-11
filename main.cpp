#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <Math.h>     // Needed for sin, cos
#include <stdio.h>
#define PI 3.14159265f

// Global variables
char title[] = "Bouncing Ball (2D)";  // Windowed mode's title
int windowWidth  = 480;     // Windowed mode's width
int windowHeight = 640;     // Windowed mode's height
int windowPosX   = 50;      // Windowed mode's top-left corner x
int windowPosY   = 50;      // Windowed mode's top-left corner y

int player1 = 0, player2 = 0;
GLfloat ballRadius = 5;   // Radius of the bouncing ball
GLfloat ballX = windowWidth / 2;         // Ball's center (x, y) position
GLfloat ballY = windowHeight / 2;
int paddle1X = windowWidth / 2;
int paddle1Y = windowHeight - 10;
int paddle2X = windowWidth / 2;
int paddle2Y = 10;
GLfloat paddleLength = 80;
GLfloat paddleBreadth = 20;
GLfloat ballXMax, ballXMin, ballYMax, ballYMin; // Ball's center (x, y) bounds
GLfloat xSpeed = 4;      // Ball's speed in x and y directions
GLfloat ySpeed = 6a;
int refreshMillis = 30;      // Refresh period in milliseconds
int movement[4] = {0, 0, 0, 0};

int a = 20, b = 60, c = 30, d = 40;

// Projection clipping area
GLdouble clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop;

/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(0.0, 0.0, 0.0, 1.0); // Set background (clear) color to black
}

void drawLine(float X1, float Y1, float X2, float Y2){
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

        glColor3f(0.0f, 0.0f, 1.0f);

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

    glBegin(GL_POINTS);
        glColor3f(0.0f, 0.0f, 1.0f);
        int x = radius, y = 0;

        glVertex2d(x + x_centre, y + y_centre);
        if (radius > 0){
            glVertex2d(x + x_centre, -y + y_centre);
            glVertex2d(y + x_centre, x + y_centre);
            glVertex2d(-y + x_centre, x + y_centre);
        }
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
            glVertex2d(x + x_centre, y + y_centre);
            glVertex2d(-x + x_centre, y + y_centre);
            glVertex2d(x + x_centre, -y + y_centre);
            glVertex2d(-x + x_centre, -y + y_centre);
            if (x != y){
                glVertex2d(y + x_centre, x + y_centre);
                glVertex2d(-y + x_centre, x + y_centre);
                glVertex2d(y + x_centre, -x + y_centre);
                glVertex2d(-y + x_centre, -x + y_centre);
            }
        }
    glEnd();
}

void drawPaddle(int length, int breadth, int centreX, int centreY) {
   drawLine(centreX - length/2, centreY + breadth/2, centreX + length/2, centreY + breadth/2);
   drawLine(centreX + length/2, centreY - breadth/2, centreX + length/2, centreY + breadth/2);
   drawLine(centreX - length/2, centreY - breadth/2, centreX + length/2, centreY - breadth/2);
   drawLine(centreX - length/2, centreY - breadth/2, centreX - length/2, centreY + breadth/2);
}

/* Callback handler for window re-paint event */
void display() {
   glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer
   glMatrixMode(GL_MODELVIEW);    // To operate on the model-view matrix
   glLoadIdentity();              // Reset model-view matrix

   drawPaddle(paddleLength, paddleBreadth, paddle1X, paddle1Y);
   drawPaddle(paddleLength, paddleBreadth, paddle2X, paddle2Y);
   drawCircle(ballX, ballY, ballRadius);

   glutSwapBuffers();  // Swap front and back buffers (of double buffered mode)

   // Animation Control - compute the location for the next refresh
   ballX += xSpeed;
   ballY += ySpeed;
   // Check if the ball exceeds the edges
   if (ballX > ballXMax) {
      ballX = ballXMax;
      xSpeed = -xSpeed;
   } else if (ballX < ballXMin) {
      ballX = ballXMin;
      xSpeed = -xSpeed;
   }

   if ((ballY - (windowHeight - paddleBreadth - ballRadius) < ySpeed) && (ballY - (windowHeight - paddleBreadth - ballRadius) >= 0)
       && ySpeed > 0 && (ballX < paddle1X + paddleLength/2 + ballRadius) && (ballX > paddle1X - paddleLength/2 - ballRadius )) {
        ySpeed = -ySpeed;
   } else if ((ballY - (paddleBreadth + ballRadius) > ySpeed) && (ballY - (paddleBreadth + ballRadius) <= 0) && ySpeed < 0 &&
              (ballX < paddle2X + paddleLength/2 + ballRadius) && (ballX > paddle2X - paddleLength/2 - ballRadius )) {
        ySpeed = -ySpeed;
   } else if (ballY > ballYMax) {
        player2++;
        ballX = windowWidth / 2 + a;
        ballY = windowHeight / 2 + b;
        a += 50;
        b += 70;
        if (ballX > windowWidth || ballX < 0) {
            ballX = 150;
        }
        if (ballY > windowHeight || ballY < 0) {
            ballY = 230;
        }
        printf("%d - %d\n", player1, player2);
   } else if (ballY < ballYMin) {
        player1++;
        ballX = windowWidth / 2 + c;
        ballY = windowHeight / 2 - d;
        c += 40;
        d += 50;
        if (ballX > windowWidth || ballX < 0) {
            ballX = 100;
        }
        if (ballY > windowHeight || ballY < 0) {
            ballY = 200;
        }
        printf("%d - %d\n", player1, player2);
   }

   if (movement[0] && !movement[1] && paddle1X - paddleLength/2 >= 0) {
        paddle1X -= 5;
   }
   else if (!movement[0] && movement[1] && paddle1X + paddleLength/2 <= windowWidth) {
        paddle1X += 5;
   }
   if (movement[2] && !movement[3] && paddle2X - paddleLength/2 >= 0) {
        paddle2X -= 5;
   }
   else if (!movement[2] && movement[3] && paddle2X + paddleLength/2 <= windowWidth) {
        paddle2X += 5;
   }
}

void specialKeys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_RIGHT:
     //    if (paddle2X + paddleLength/2 <= windowWidth){
     //       paddle2X += 5;
     //    }
         movement[3] = 1;
         break;
      case GLUT_KEY_LEFT:
    //     if (paddle2X - paddleLength/2 >= 0){
     //       paddle2X -= 5;
     //    }
         movement[2] = 1;
         break;
   }
}

void specialUpKeys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_RIGHT:
     //    if (paddle2X + paddleLength/2 <= windowWidth){
     //       paddle2X += 5;
     //    }
         movement[3] = 0;
         break;
      case GLUT_KEY_LEFT:
    //     if (paddle2X - paddleLength/2 >= 0){
     //       paddle2X -= 5;
     //    }
         movement[2] = 0;
         break;
   }
}

void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 97:
   //      if (paddle1X - paddleLength/2 >= 0){
   //         paddle1X -= 5;
   //      }
         movement[0] = 1;
         break;
      case 100:
    //     if (paddle1X + paddleLength/2 <= windowWidth){
    //        paddle1X += 5;
    //     }
         movement[1] = 1;
         break;
   }
}

void keyboardUp(unsigned char key, int x, int y) {
   switch (key) {
      case 97:
    //     if (paddle1X - paddleLength/2 >= 0){
    //        paddle1X -= 5;
    //     }
         movement[0] = 0;
         break;
      case 100:
   //      if (paddle1X + paddleLength/2 <= windowWidth){
   //         paddle1X += 5;
   //      }
         movement[1] = 0;
         break;
   }
}

/* Call back when the windows is re-sized */
void reshape(GLsizei width, GLsizei height) {

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset the projection matrix

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
   paddle1Y = height - paddleBreadth/2;
}

/* Called back when the timer expired */
void Timer(int value) {
   glutPostRedisplay();    // Post a paint request to activate display()
   glutTimerFunc(refreshMillis, Timer, 0); // subsequent timer call at milliseconds
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
   glutInitWindowSize(windowWidth, windowHeight);  // Initial window width and height
   glutInitWindowPosition(windowPosX, windowPosY); // Initial window top-left corner (x, y)
   glutCreateWindow(title);      // Create window with given title
   glutDisplayFunc(display);     // Register callback handler for window re-paint
   glutReshapeFunc(reshape);     // Register callback handler for window re-shape
   glutSpecialFunc(specialKeys);
   glutKeyboardFunc(keyboard);
   glutSpecialUpFunc(specialUpKeys);
   glutKeyboardUpFunc(keyboardUp);
   glutTimerFunc(0, Timer, 0);   // First timer call immediately
   initGL();                     // Our own OpenGL initialization
   glutMainLoop();               // Enter event-processing loop
   return 0;
}
