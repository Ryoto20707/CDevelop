#ifndef __MAP_H__
#define __MAP_H__

#if _WIN32
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#define PI 3.14159
#define M 10.0
#define INERTIA 4.0
#define TIMELIMIT 60

/*
 * map.cのbelt[]のインデックス
 */
#define X_FROM    0 // x座標（から）
#define X_TO      1 // x座標（まで）
#define Y_FROM    2 // y座標 (から)
#define Y_TO      3 // y座標（まで）
#define SPEED     4 // 速さ（DIRECTION軸に対して正の向き）
#define DIRECTION 5 // 方向

/*
 * DIRECTIONの取りうる値
 */
#define X_DIR 1.0 // x軸方向
#define Y_DIR 2.0 // y軸方向

void calcNormal(GLdouble[3], GLdouble[3], GLdouble[3], GLdouble[3]);
void drawIcePanel(GLdouble, GLdouble);
void drawSand(GLdouble, GLdouble, GLdouble);
void drawSandPanel(GLdouble, GLdouble);
void drawGround();
void drawBelt();
void obstacle();
void drawJiki();
void drawTeki();
void mapDisplay();

GLfloat pos0[], pos1[];
GLfloat color[][4];
double x, y, z;
int mySpecialValue;
GLdouble vertex1[][3];
int face[][4];
double tekiList[][3];
int tekiIndex;
double v;
int X;
int Y;
double L;
double friction[][5];
int fricindex;
double belt[][6];
int beltindex;
double theta;

#endif
