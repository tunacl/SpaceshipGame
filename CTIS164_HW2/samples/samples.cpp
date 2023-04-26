/*****
   CTIS164 - Lab08
   Trigonometry - Basics
*****
   ver03: Target is added
*****/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

#define  TIMER_PERIOD  16 // 60fps
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS 20

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height

/* User defined Types and Variables */
typedef struct {
    float x, y;
} point_t;

typedef struct {
    point_t pos;    // position of the object
    float   angle;  // view angle 
    float   r;
} player_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} fire_t;

typedef struct {
    int r, g, b;
} color_t;

typedef struct {
    point_t center;
    color_t color;
    float radius;
    float speed;
} target_t;

#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 8 // after 8 frames you can throw another one.

player_t p = { {0,0}, 45, 20 };
fire_t   fr[MAX_FIRE];
int fire_rate = 0;
target_t target;


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

void drawBg() {
    glColor3f(0.4, 0.4, 0.4);  // line color
    glLineWidth(3);   // line thickness
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(3, 0xf0f0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-200, -200);
    glVertex2f(-200, 200);
    glVertex2f(200, 200);
    glVertex2f(200, -200);
    glEnd();
    glLineWidth(1);   // line thickness
    glDisable(GL_LINE_STIPPLE);

    glColor3f(0.6, 0.6, 0);
    glBegin(GL_LINES);
    // x axis
    glVertex2f(-200, 0);
    glVertex2f(200, 0);

    // y axis
    glVertex2f(0, -200);
    glVertex2f(0, 200);
    glEnd();
}

void drawPlayer(player_t tp) {
    glColor3f(1, 1, 0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(tp.pos.x, tp.pos.y);
    glVertex2f(tp.pos.x + 50 * cos(tp.angle * D2R), tp.pos.y + 50 * sin(tp.angle * D2R));
    glEnd();
    glLineWidth(1);

    glColor3f(0.3, 0.3, 1.0);
    circle(tp.pos.x, tp.pos.y, tp.r);
    glColor3f(1.0, 0.3, 0.3);
    circle(tp.pos.x, tp.pos.y, tp.r - 4);

    glColor3f(1, 1, 1);
    vprint(tp.pos.x - 12, tp.pos.y - 5, GLUT_BITMAP_8_BY_13, "%.0f", tp.angle);
}

void drawFires() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            glColor3f(1, 1, 1);
            circle(fr[i].pos.x, fr[i].pos.y, 6);
        }
    }
}

void drawTarget(target_t t) {
    glColor3ub(t.color.r, t.color.g, t.color.b);
    circle(t.center.x, t.center.y, t.radius);
}

void display()
{

    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    drawBg();
    drawFires();
    drawPlayer(p);
    drawTarget(target);

    glutSwapBuffers();

}

// In the fire array, check if any fire is available.
// if there is a fire that we can use for firing, return its index.
int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false) return i;
    }
    return -1;
}

void resetTarget() {
    float yPos = rand() % 2 ? 225 : -225;
    target.center = { -300, yPos };
    target.color.r = rand() % 256;
    target.color.g = rand() % 256;
    target.color.b = rand() % 256;
    target.radius = TARGET_RADIUS;
    target.speed = 3.0 * (rand() % 100) / 100.0 + 0.5;
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = true;
    // to refresh the window it calls display() function
    //glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = false;
    // to refresh the window it calls display() function
    //glutPostRedisplay() ;
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
    // to refresh the window it calls display() function
    //glutPostRedisplay() ;
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
    //glutPostRedisplay() ;
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



    // to refresh the window it calls display() function
    //glutPostRedisplay() ; 
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
    //glutPostRedisplay() ;
}

void onMove(int x, int y) {
    // Write your codes here.



     // to refresh the window it calls display() function
     //glutPostRedisplay() ;
}

void turnPlayer(player_t* tp, float inc) {
    tp->angle += inc;
    if (tp->angle > 360) tp->angle -= 360;
    if (tp->angle < 0) tp->angle += 360;
}

void movePlayer(player_t* tp, float speed) {
    float dx = speed * cos(tp->angle * D2R);
    float dy = speed * sin(tp->angle * D2R);

    tp->pos.x += dx;
    tp->pos.y += dy;
    float r = tp->r;

    // check rectangular boundary
    if (tp->pos.x > (200 - r) || tp->pos.x < (-200 + r) ||
        tp->pos.y >(200 - r) || tp->pos.y < (-200 + r)) {
        tp->pos.x -= dx;
        tp->pos.y -= dy;
    }
}

bool testCollision(fire_t fr, target_t t) {
    float dx = t.center.x - fr.pos.x;
    float dy = t.center.y - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= t.radius;
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    if (right) {
        turnPlayer(&p, -3);
    }
    if (left) {
        turnPlayer(&p, 3);
    }
    if (up) {
        movePlayer(&p, 3);
    }
    if (down) {
        movePlayer(&p, -3);
    }

    if (spacebar && fire_rate == 0) {
        int availFire = findAvailableFire();
        if (availFire != -1) {
            fr[availFire].pos = p.pos;
            fr[availFire].angle = p.angle;
            fr[availFire].active = true;
            fire_rate = FIRE_RATE;
        }
    }

    if (fire_rate > 0) fire_rate--;

    // Move all fires that are active.
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            fr[i].pos.x += 10 * cos(fr[i].angle * D2R);
            fr[i].pos.y += 10 * sin(fr[i].angle * D2R);

            if (fr[i].pos.x > 250 || fr[i].pos.x < -250 || fr[i].pos.y>250 || fr[i].pos.y < -250) {
                fr[i].active = false;
            }

            if (testCollision(fr[i], target)) {
                fr[i].active = false;
                resetTarget();
            }
        }
    }

    // move target from left to right
    target.center.x += target.speed;
    if (target.center.x > 275) {
        resetTarget();
    }
    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    resetTarget();
}


void main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Move in Angle Direction");

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