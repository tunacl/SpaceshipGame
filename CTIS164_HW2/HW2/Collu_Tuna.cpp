/*********
   CTIS164 - Template Source Program
----------
STUDENT : TUNA ÇÖLLÜ
SECTION :02
HOMEWORK:HW2
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:If you hit all 50 targets, you will win but if any bullet sliding from top is reached the border, you'll lose the game and program count the hits and time. After the 20th and 43rd 
score, the spaceship and the bullets falling from the top of the screen begin to accelerate. 
                                
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int xD, xT1, xT2, xT3, xY, xA1, xA2, xA3, xB, yB, xER, yER, xET, yET;
int second, j=0, msecond, i=0, score=0;

bool active;
bool offscreen = false;
bool win = false;
typedef struct {
    int r, g, b;
} color_t;
typedef struct {
    int x;
    int y;
    int activated;
    color_t color;
}targets_t;
targets_t targets[50];

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void changecolor() {
    for (i = 0; i <= 50; i++)
    {
        targets[i].color.r = rand() % 256;
        targets[i].color.g = rand() % 256;
        targets[i].color.b = rand() % 256;
    }
}
void enemy()
{
      
    for (i = 0; i <= 50; i++)
        if (targets[i].activated == true)
        {
            glColor3ub(targets[i].color.r, targets[i].color.g, targets[i].color.b);
            glRectf(targets[i].x + 100, targets[i].y + 100, targets[i].x + 130, targets[i].y + 50);

            glColor3ub(targets[i].color.g, targets[i].color.b, targets[i].color.r);
            glBegin(GL_TRIANGLES);
            glVertex2f(targets[i].x + 100, targets[i].y + 50);
            glVertex2f(targets[i].x + 130, targets[i].y + 50);
            glVertex2f(targets[i].x + 115, targets[i].y + 30);
            glEnd();

        }

}
void bullet()
{
    glColor3ub(173 ,255 ,47);
    circle(xB+0, yB-210, 5);
}
void spaceship()
{  
    //body
    glColor3ub(255 ,218 ,185);
    glRectf(xD-30, -250, xD+30, -330);

    //up triangle
    glColor3ub(255, 192, 203);
    glBegin(GL_TRIANGLES);
    glVertex2f(xT1-30, -250);
    glVertex2f(xT1+0, -220);
    glVertex2f(xT1+30, -250);
    glEnd();

    //left triangle
    glColor3ub(255, 192, 203);
    glBegin(GL_TRIANGLES);
    glVertex2f(xT2-30, -330);
    glVertex2f(xT2-50, -330);
    glVertex2f(xT2-30, -290);
    glEnd();

    //right triangle
    glColor3ub(255, 192, 203);
    glBegin(GL_TRIANGLES);
    glVertex2f(xT3+30, -330);
    glVertex2f(xT3+50, -330);
    glVertex2f(xT3+30, -290);
    glEnd();

    //body circle
    glColor3ub(255, 105, 180);
    circle(xY+0, -270, 15);

    //fire
    glLineWidth(3);
    glColor3ub(255 ,0 ,0);
    glBegin(GL_LINES);
    glVertex2f(xA1-10, -330);
    glVertex2f(xA1-15, -345);

    glVertex2f(xA2+0, -330);
    glVertex2f(xA2+0, -345);

    glVertex2f(xA3+10, -330);
    glVertex2f(xA3+15, -345);

   
    glEnd();
    
}

void overscreen()//gameover screen
{
    for (i = 0; i <= 50; i++)
    {
        if (targets[i].y + 30 <= -200 && targets[i].activated == true)
        {
            offscreen = true;
        }
    }
    
    if (offscreen==true) {
        glColor3ub(0, 0, 0);
        glRectf(-1000, 800, 1000, -800);
        glColor3ub(255, 255, 255);
        vprint2(-100, 0, 0.3, "GAME OVER!");
    }
}
void winscreen()
{
    for (i = 0; i <= 50; i++)
    {
        if (score == 50 && targets[i].activated == true)
        {
            win = true;
        }
    }

    if (win == true) {
        glColor3ub(0, 0, 0);
        glRectf(-1000, 800, 1000, -800);
        glColor3ub(255, 255, 255);
        vprint2(-600, 0, 0.3, "CONGRATULATIONS YOU'VE DESTROYED ALL THE ENEMIES!");
    }
}
void informations()//score,time,name
{
    glColor3ub(255, 255, 255);
    vprint2(-700, -350, 0.2, "Score: %d", score);
    vprint(500, -300, GLUT_BITMAP_HELVETICA_18, "How to Play?");
    vprint(430, -320, GLUT_BITMAP_HELVETICA_18, "Use directional arrows to move");
    vprint(490, -340, GLUT_BITMAP_HELVETICA_18, "Space to shoot");
    vprint(390, -360, GLUT_BITMAP_HELVETICA_18, "If any bullet crosses the red line you lose!");
    vprint2(550, 300, 0.2, "Time:%d seconds", second);   
    vprint(-720, 350, GLUT_BITMAP_TIMES_ROMAN_24, "Tuna COLLU");
    vprint(-720, 325, GLUT_BITMAP_TIMES_ROMAN_24, "Section-02");
    vprint(-720, 300, GLUT_BITMAP_TIMES_ROMAN_24, "21901544");
    vprint(-720, 275, GLUT_BITMAP_TIMES_ROMAN_24, "HOMEWORK #2");
    
}
void border()//red line
{
    glColor3ub(255, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(-1000, -200);
    glVertex2f(1000, -200);
    glEnd();

}
//
// To display onto window using OpenGL commands
//
void display() 
{    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

   
    spaceship();
    enemy();
    bullet();
    informations();    
    border();
    overscreen();
    winscreen();
    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == 32&& yB==0)
    {
        active = !active;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }
    if (key == GLUT_KEY_LEFT)
    {
        xD -= 10;
        xT1 -= 10;
        xT2 -= 10;
        xT3 -= 10;
        xA1 -= 10;
        xA2 -= 10;
        xA3 -= 10;
        xY -= 10;
        if (yB == 0)
            xB -= 10;

        if (score >= 20)//faster 
        {
            xD -= 15;
            xT1 -= 15;
            xT2 -= 15;
            xT3 -= 15;
            xA1 -= 15;
            xA2 -= 15;
            xA3 -= 15;
            xY -= 15;
            if (yB == 0)
                xB -= 15;

            if (score >= 43)
            {
                xD -= 23;
                xT1 -= 23;
                xT2 -= 23;
                xT3 -= 23;
                xA1 -= 23;
                xA2 -= 23;
                xA3 -= 23;
                xY -= 23;
                if (yB == 0)
                    xB -= 23;

            }
        }

    }
    if (key == GLUT_KEY_RIGHT)
    {
        xD += 10;
        xT1 += 10;
        xT2 += 10;
        xT3 += 10;
        xA1 += 10;
        xA2 += 10;
        xA3 += 10;
        xY += 10;
        if (yB == 0)
            xB += 10;
        if (score >= 20)//faster 
        {
            xD += 15;
            xT1 += 15;
            xT2 += 15;
            xT3 += 15;
            xA1 += 15;
            xA2 += 15;
            xA3 += 15;
            xY += 15;
            if (yB == 0)
                xB += 15;
            if (score >= 43)
            {
                xD += 23;
                xT1 += 23;
                xT2 += 23;
                xT3 += 23;
                xA1 += 23;
                xA2 += 23;
                xA3 += 23;
                xY += 23;
                if (yB == 0)
                    xB += 23;

            }
        }

    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.

    x = x - winWidth / 2;
    y = winHeight / 2 - y;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}



void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.
    
   
   
    //randomly generating enemies
    if (second % 3 == 0 && msecond % 1000 == 0)
    {    
        targets[j].activated = true;
        targets[j].x = rand() % ((600 + 1) - (-900)) - 900;
        targets[j].y = 400;
        j++;
    }

    //enemies falldown speed
    for (i = 0; i < 50; i++)
    {
        if (targets[i].activated == true)
        {
            targets[i].y -= 1;
            
        }
        if (score >= 20)
        {
            targets[i].y -= 1.25;
           
            if (score >= 43)
            {
                targets[i].y -= 1.5;
               
            }
           
        }
    }
   
    if (active)//spaceship's bullet speed
    {
        yB += 10;
    }


    if (yB == 600)//spaceship bullet's respawn
    {
        xB = xT1;
        yB = 0;
        active = !active;
    }
     
    msecond += 20;
    if (msecond % 1000 == 0)
        second++;

    //disabled when hit
    for (i = 0; i < 50; i++)
    {
        if (yB - 210 <= targets[i].y + 100 && yB - 210 >= targets[i].y + 30 && xB >= targets[i].x + 90 && xB <= targets[i].x + 140)
        {
            if (targets[i].activated == true)
            {
                score++;
            }

                targets[i].activated = false;
            

        }
    }



    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {
   


    second = 0;
    msecond = 0;
    changecolor();
    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("PROTECTING SPACE");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}