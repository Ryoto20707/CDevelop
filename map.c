#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>
#include "belt.h"
#define PI (3.14159)
#define M 10.0
#define INERTIA 4.0
#define TIMELIMIT 1000000


GLfloat pos0[] = { 5.0, 0.0, 0.0, 1.0 };
GLfloat pos1[] = { 0.0, 0.0, 5.0, 1.0 };
enum COLOR { WHITE, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, GRAY, BLACK, YGREEN, BROWN,BELT_BLACK, BELT_GRAY, BELT_SILVER };
GLfloat color[][4] = {
    { 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 0.0, 0.0, 1.0 },
    { 0.0, 1.0, 0.0, 1.0 },
    { 0.0, 0.0, 1.0, 1.0 },
    { 1.0, 1.0, 0.0, 1.0 },
    { 1.0, 0.0, 1.0, 1.0 },
    { 0.0, 1.0, 1.0, 1.0 },
    { 0.7, 0.7, 0.7, 1.0 },
    { 0.0, 0.0, 0.0, 1.0 },
    { 0.5, 1.0, 0.0, 1.0 },
    { 0.58, 0.28, 0.1, 1.0 },
    { 0.2, 0.2, 0.2, 1.0 },
    { 0.5, 0.5, 0.5, 1.0 },
    { 0.77, 0.77, 0.77, 1.0 } };//色を増やす場合はここに追加
double x = 0;
double y = 0;
double z = 0;
int mySpecialValue = 0;

GLdouble vertex1[][3] = {//直方体(木の幹)の座標
	{ -0.1, -0.1, 0.01 },
	{ 0.1, -0.1, 0.01 },
	{ 0.1, 0.1, 0.01 },
	{ -0.1, 0.1, 0.01 },
	{ -0.1, -0.1, 0.5 },
	{ 0.1, -0.1, 0.5 },
	{ 0.1, 0.1, 0.5 },
	{ -0.1, 0.1, 0.5 }
};

int face[][4] = {//面の定義。直方体生成時に必要
	{ 0, 1, 2, 3 },
	{ 1, 5, 6, 2 },
	{ 5, 4, 7, 6 },
	{ 4, 0, 3, 7 },
	{ 4, 5, 1, 0 },
	{ 3, 2, 6, 7 }
};

double tekiList[][3] = {
	{ 0.0, 2.0, 0.0 },
	{ 4.0, 4.0, 0.0 },
	{ 2.0, 6.0, 0.0 },
	{ 0.0, 10.0, 0.0 },
	{ 4.0, 13.0, 0.0 },
	{ 3.0, 14.0, 0.0 },
	{ 2.0, 15.0, 0.0 },
	{ 1.0, 16.0, 0.0 },
	{ 3.0, 19.0, 0.0 },
	{ 2.0, 20.0, 0.0 },
	{ 1.0, 21.0, 0.0 },
	{ 0.0, 22.0, 0.0 }
};//障害物のリスト
int tekiIndex = 12;
double v = 0;

int X = 5;//地面
int Y = 25;
double L = 1;

double friction[][5] = {
	{ 0.0, 5.0, 0.0, 25.0, 0.1}, //デフォルト値(変えないで！)
	{ 2.0, 3.0, 15.0, 25.0, 0.05},
	{ 4.0, 5.0, 0.0, 5.0, 0.3}
}; //床の摩擦。x座標からx座標まで(範囲)、　y座標からy座標まで、速さ
int fricindex = 3;

double belt[][6] = {
	{ 0.0, 5.0, 0.0, 25.0, 0.0, 0.0}, //デフォルト値(変えないで！)
    { 0.5, 4.5, 0.5, 3.5, 0.05, X_DIR},
    { 2.5, 3.5, 15.5, 20.5, -0.02, Y_DIR}
}; //ベルトコンベア。x座標からx座標まで(範囲)、　y座標からy座標まで、速さ、向き(+x:1,+y:2)
int beltindex = 3;

void calcNormal(GLdouble v0[3], GLdouble v1[3], GLdouble v2[3], GLdouble n[3])
{
	GLdouble v2v[2][3];
	GLdouble vt[3];
	double abs;
	int i;
	for (i = 0; i < 3; i++)
	{
		v2v[0][i] = v1[i] - v0[i];
		v2v[1][i] = v2[i] - v0[i];
	}
	vt[0] = v2v[0][1] * v2v[1][2] - v2v[0][2] * v2v[1][1];
	vt[1] = v2v[0][2] * v2v[1][0] - v2v[0][0] * v2v[1][2];
	vt[2] = v2v[0][0] * v2v[1][1] - v2v[0][1] * v2v[1][0];
	abs = sqrt(vt[0] * vt[0] + vt[1] * vt[1] + vt[2] * vt[2]);
	for (i = 0; i < 3; i++)
		n[i] = vt[i] / abs;
}

void drawGround()
{
	int i, j;
	GLdouble normal[3] = { 0.0, 0.0, 1.0 };

	glPushMatrix();

	glNormal3dv(normal);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[YELLOW]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BLACK]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glBegin(GL_QUADS);
	glVertex3d(-0.5 * L, -0.5 * L, 0.0); //スタート
	glVertex3d((X - 0.5)*L, -0.5 * L, 0.0);
	glVertex3d((X - 0.5)*L, 0.5 * L, 0.0);
	glVertex3d(-0.5 * L, 0.5 * L, 0.0);

	glVertex3d(-0.5 * L, (Y - 0.5)*L, 0.0); //ゴール
	glVertex3d((X - 0.5)*L, (Y - 0.5)*L, 0.0);
	glVertex3d((X - 0.5)*L, (Y + 0.5)*L, 0.0);
	glVertex3d(-0.5 * L, (Y + 0.5)*L, 0.0);
	glEnd();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[GRAY]);//灰色
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BLACK]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glBegin(GL_QUADS);
	for (i = 1; i < Y; i++)
		for (j = 0; j < X; j++)
		{
			GLdouble v[4][3];
			if (!((i ^ j) & 1))continue;
			v[0][0] = (j + 0 - 0.5) * L;
			v[0][1] = (i + 0 - 0.5) * L;
			v[0][2] = 0;
			v[1][0] = (j + 1 - 0.5) * L;
			v[1][1] = (i + 0 - 0.5) * L;
			v[1][2] = 0;
			v[2][0] = (j + 1 - 0.5) * L;
			v[2][1] = (i + 1 - 0.5) * L;
			v[2][2] = 0;
			v[3][0] = (j + 0 - 0.5) * L;
			v[3][1] = (i + 1 - 0.5) * L;
			v[3][2] = 0;
			glVertex3dv(v[0]);
			glVertex3dv(v[1]);
			glVertex3dv(v[2]);
			glVertex3dv(v[3]);
		}
	glEnd();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[BLACK]);//黒
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BLACK]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glBegin(GL_QUADS);
	for (i = 0; i < Y; i++)
		for (j = 0; j < X; j++)
		{
			GLdouble v[4][3];
			if ((i ^ j) & 1)continue;
			v[0][0] = (j + 0 - 0.5) * L;
			v[0][1] = (i + 0 - 0.5) * L;
			v[0][2] = 0;
			v[1][0] = (j + 1 - 0.5) * L;
			v[1][1] = (i + 0 - 0.5) * L;
			v[1][2] = 0;
			v[2][0] = (j + 1 - 0.5) * L;
			v[2][1] = (i + 1 - 0.5) * L;
			v[2][2] = 0;
			v[3][0] = (j + 0 - 0.5) * L;
			v[3][1] = (i + 1 - 0.5) * L;
			v[3][2] = 0;
			glVertex3dv(v[0]);
			glVertex3dv(v[1]);
			glVertex3dv(v[2]);
			glVertex3dv(v[3]);
		}
	glEnd();
    
    drawBelt(); //ベルトコンベアの描写

	glPopMatrix();
}

void drawBelt(void){ //ベルトコンベアの描写
    int i = 0, j = 0;
    double xp = 0.0, yp = 0.0;
    GLdouble normal[3] = { 0.0, 0.0, 1.0 };
    
    glPushMatrix();
    
    glNormal3dv(normal);
    
    /* ベルトコンべアの描画 */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color[BELT_GRAY]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, color[BELT_GRAY]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
    
    glBegin(GL_QUADS);
    for(i = 1; i < beltindex; i++){
        glVertex3d( belt[i][X_FROM], belt[i][Y_FROM], 0.005 );
        glVertex3d( belt[i][X_TO], belt[i][Y_FROM] , 0.005 );
        glVertex3d( belt[i][X_TO], belt[i][Y_TO] , 0.005 );
        glVertex3d( belt[i][X_FROM], belt[i][Y_TO] , 0.005 );
    }
    glEnd();
    
    /* ベルトコンベアの縁の描画 */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color[BELT_SILVER]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, color[BELT_SILVER]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
    glLineWidth(2.0);
    for(i = 1; i < beltindex; i++){
        glBegin(GL_LINE_LOOP);
        glVertex3d( belt[i][X_FROM], belt[i][Y_FROM], 0.01 );
        glVertex3d( belt[i][X_TO], belt[i][Y_FROM] , 0.01 );
        glVertex3d( belt[i][X_TO], belt[i][Y_TO] , 0.01 );
        glVertex3d( belt[i][X_FROM], belt[i][Y_TO] , 0.01 );
        glEnd();
        
        if(belt[i][DIRECTION] ==  X_DIR){ //x方向のベルトコンベアの場合
            for(yp = belt[i][Y_FROM] + 1.0; yp < belt[i][Y_TO] ; yp+= 1.0){
                glBegin(GL_LINES);
                glVertex3d( belt[i][X_FROM], yp, 0.01 );
                glVertex3d( belt[i][X_TO], yp, 0.01 );
                glEnd();
            }
        }else if(belt[i][DIRECTION] ==  Y_DIR){ //y方向のベルトコンベアの場合
            for(xp = belt[i][X_FROM] + 1.0; xp < belt[i][X_TO] ; xp+= 1.0){
                glBegin(GL_LINES);
                glVertex3d( xp, belt[i][Y_FROM], 0.01 );
                glVertex3d( xp, belt[i][Y_TO], 0.01 );
                glEnd();
            }
        }
    }
    
    /* 矢印の描画 */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color[BELT_BLACK]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, color[BELT_BLACK]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
    
    glLineWidth(3.0);
    for(i = 1; i < beltindex; i++){
        for(yp = belt[i][Y_FROM]; yp < belt[i][Y_TO]; yp += 1.0){
            for(xp = belt[i][X_FROM]; xp < belt[i][X_TO]; xp += 1.0){
                GLdouble ArrowVertex[3][3]; //三角形の頂点を格納
                
                //三角形の向きを決定させる
                if(belt[i][DIRECTION] ==  X_DIR){ //x方向のベルトコンベアの場合
                    if(belt[i][SPEED] > 0.0){ //速度が正の場合
                        ArrowVertex[0][0] = xp, ArrowVertex[0][1] = yp, ArrowVertex[0][2] = 0.01;
                        ArrowVertex[1][0] = xp + 1.0, ArrowVertex[1][1] = yp + 0.5, ArrowVertex[1][2] = 0.01;
                        ArrowVertex[2][0] = xp, ArrowVertex[2][1] = yp + 1.0, ArrowVertex[2][2] = 0.01;
                    }else if(belt[i][SPEED] < 0.0){ //速度が負の場合
                        ArrowVertex[0][0] = xp + 1.0, ArrowVertex[0][1] = yp + 1.0, ArrowVertex[0][2] = 0.01;
                        ArrowVertex[1][0] = xp, ArrowVertex[1][1] = yp + 0.5, ArrowVertex[1][2] = 0.01;
                        ArrowVertex[2][0] = xp + 1.0, ArrowVertex[2][1] = yp, ArrowVertex[2][2] = 0.01;
                    }
                }else if(belt[i][DIRECTION] == Y_DIR){ //y方向のベルトコンベアの場合
                    if(belt[i][SPEED] > 0.0){ //速度が正の場合
                        ArrowVertex[0][0] = xp + 1.0, ArrowVertex[0][1] = yp, ArrowVertex[0][2] = 0.01;
                        ArrowVertex[1][0] = xp + 0.5, ArrowVertex[1][1] = yp + 1.0, ArrowVertex[1][2] = 0.01;
                        ArrowVertex[2][0] = xp, ArrowVertex[2][1] = yp, ArrowVertex[2][2] = 0.01;
                    }else if(belt[i][SPEED] < 0.0){ //速度が負の場合
                        ArrowVertex[0][0] = xp, ArrowVertex[0][1] = yp + 1.0,  ArrowVertex[0][2] = 0.01;
                        ArrowVertex[1][0] = xp + 0.5, ArrowVertex[1][1] = yp, ArrowVertex[1][2] = 0.01;
                        ArrowVertex[2][0] = xp + 1.0, ArrowVertex[2][1] = yp + 1.0, ArrowVertex[2][2] = 0.01;
                    }
                }
                
                glBegin(GL_LINE_STRIP);
                for(j = 0; j < 3; j++) glVertex3dv(ArrowVertex[j]); //三角形を描写
                glEnd();
            }
        }
    }
    
    glPopMatrix();
}

void obstacle1(void)	//障害物（木）
{
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[BROWN]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BROWN]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glBegin(GL_QUADS);
	for (int j = 0; j < 6; ++j) {
		for (int i = 0; i < 4; ++i) {
			glVertex3dv(vertex1[face[j][i]]);
		}
	}
	glEnd();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[YGREEN]);
	glTranslatef(0, 0, 0.5);
	glutSolidCone(0.5, 0.75, 20, 20);
	glTranslatef(0, 0, 0.25);
	glutSolidCone(0.4, 0.6, 20, 20);
	glTranslatef(0, 0, 0.25);
	glutSolidCone(0.3, 0.45, 20, 20);
	glPopMatrix();
}

double theta;
void drawJiki(void)		//動かす物
{
	glPushMatrix();

	glTranslatef(x, y, z);
	glTranslatef(0.0, 0.0, 0.5);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[GREEN]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BLACK]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

	//glutSolidCube(1);
	glutSolidSphere(0.5, 30, 30);
	glPopMatrix();
}
void drawTeki(void)		//障害物の一括配置
{
	int i;
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color[RED]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, color[BLACK]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color[WHITE]);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);

	for (i = 0; i < tekiIndex; i++)
	{
		glPushMatrix();
		glTranslatef(tekiList[i][0], tekiList[i][1], tekiList[i][2]);
		//glutSolidCone(0.5, 1, 30, 30);
		obstacle1();					//ここで木を配置する
		glPopMatrix();
	}
	glPopMatrix();
}

void mapdisplay(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1, .1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, -10.0, 2.0, 0.0, 0.0, 1.5, 0.0, 0.0, 1.0);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color[WHITE]);

}

/*void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	drawGround();
	drawJiki();
	drawTeki();

	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("C Dev");
	glutDisplayFunc(display);
	mapdisplay();
	glutMainLoop();
	return 0;
}*/