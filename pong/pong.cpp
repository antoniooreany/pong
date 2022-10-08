// pong.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// constants
constexpr auto VK_W = 0x57;
constexpr auto VK_S = 0x53;
constexpr auto M_PI = 3.14159265359;

#include <windows.h>
#include <sstream>
#include "GL\freeglut.h"
#pragma comment(lib, "OpenGL32.lib")

// window
int window_width = 1000;
int window_height = 400; 
int update_period = 1000 / 60; // frequency = 60 fps

// score
int score_left = 0;
int score_right = 0;

// racket
int racket_width = 20;
int racket_height = 160;
int racket_speed = 5;

// left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;
// right racket position
float racket_right_x = window_width - racket_width - 10.0f;
float racket_right_y = 50.0f;

// ball
float ball_pos_x = window_width / 2;
float ball_pos_y = window_height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 16;
int ball_speed = 2;

void drawText(float x, float y, std::string text) 
{
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

void drawRect(float x, float y, float width, float height) 
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float ball_pos_x, float ball_pos_y, float ball_radius)
{
    glBegin(GL_POLYGON);
    int optimization_coefficient = 3;
    int count = ball_radius * optimization_coefficient; 
    for (int i = 0; i <= count; i++) {
        double angle = 2 * M_PI * i / count;
        double x = cos(angle) * ball_radius;
        double y = sin(angle) * ball_radius;
        glVertex2d(ball_pos_x + x, ball_pos_y + y);
    }
    glEnd();
}

std::string int2str(int x) 
{
    // converts int to string
    std::stringstream ss;
    ss << x;
    return ss.str();
}

void draw() 
{
    // clear (has to be done at the beginning)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw rackets
    drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
    drawRect(racket_right_x, racket_right_y, racket_width, racket_height);
    // TODO draw beautiful rackets, which ones?

    // draw ball
    drawCircle(ball_pos_x, ball_pos_y, ball_size / 2);

    // draw score
    drawText(window_width / 2 - 30, 15,
        int2str(score_left) + ":" + int2str(score_right));
    // TODO draw beautiful score
    // TODO which one?

    // swap buffers (has to be done at the end)
    glutSwapBuffers();
}

void keyboard() 
{
    // left racket
    if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;
    // right racket
    if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}

void vec2_norm(float& x, float& y) 
{
    // sets a vectors length to 1 (which means that x + y == 1)
    float length = sqrt((x * x) + (y * y));
    if (length != 0.0f) {
        length = 1.0f / length;
        x *= length;
        y *= length;
    }
}

void updateBall() 
{
    // fly a bit
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;

    // hit by left racket?
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = t;
    }

    // hit by right racket?
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
    }

    // hit left wall?
    if (ball_pos_x < 0) {
        ++score_right;
        ball_pos_x = window_width / 2;
        ball_pos_y = window_height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
    }

    // hit right wall?
    if (ball_pos_x > window_width) {
        ++score_left;
        ball_pos_x = window_width / 2;
        ball_pos_y = window_height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
    }

    // hit top wall?
    if (ball_pos_y > window_height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }

    // hit bottom wall?
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }

    // make sure that length of dir stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);
}

void update(int value) 
{
    // input handling
    keyboard();

    // update ball
    updateBall();

    // Call update() again in 'interval' milliseconds
    glutTimerFunc(update_period, update, 0);

    // Redisplay frame
    glutPostRedisplay();
}

void enable2D(int width, int height) 
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// program entry point
int main(int argc, char** argv)
{
    // initialize opengl (via glut)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Pong game");

    // Register callback functions  
    glutDisplayFunc(draw);
    glutTimerFunc(update_period, update, 0);

    // setup scene to 2d mode and set draw color to blue
    enable2D(window_width, window_height);
    glColor3f(0.0f, 0.0f, 1.0f);

    // start the whole thing
    glutMainLoop();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
