#include <time.h>
#include <string.h>
#include "map.c"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400
#define FONT GLUT_BITMAP_9_BY_15

int collision(); //衝突判定
void myTimerFunc(int value); //プレイヤーを動かす
void myKeyboardFunc(unsigned char key, int x, int y); //キーボード操作
void mySpcialFunc(int key, int x, int y);
void mySpcialUpFunc(int key, int x, int y);
void display(); //glutMainloop()で繰り返し処理する関数群
void idle();
void init();
void replay(); //プレイヤーを初期位置に戻す
void timeKeeper(); //制限時間を管理する
void gameover(); //ゲームオーバーにする
void gameClear(); //クリア処理をする
void drawString(); //文字列の描画

char message[36] = "Start!"; // メッセージエリアに表示する文字列

int collision()
{
	//衝突判定
	int i;
	double MARGIN = 0.05;
	if (z > 1)return 0;
	for (i = 0; i < tekiIndex; i++)
	{
		//簡単な衝突判定
		if ((tekiList[i][0] - x <1 - MARGIN) && (tekiList[i][0] - x >-1 + MARGIN)
			&& (tekiList[i][1] - y <1 - MARGIN) && (tekiList[i][1] - y >-1 + MARGIN))
		{
			//printf("(%.02f,%.02f):(%.02f,%.02f)\n", x, y, tekiList[i][0], tekiList[i][1]);
			gameover();
			return 1;
		}
	}
	return 0;
}


void myTimerFunc(int value)
{
	double MARGIN = 0.05;
	int i;
	int fric = 0;
	for(i=0;i<fricindex;i++){
		if(friction[i][0]<x&&x<friction[i][1]&&friction[i][2]<y&&y<friction[i][3]) fric = i;
	}
	int bt = 0;
	for(i=0;i<beltindex;i++){
		if(belt[i][X_FROM]<x&&x<belt[i][X_TO]&&belt[i][Y_FROM]<y&&y<belt[i][Y_TO]) bt = i;
	}
	int bt_direction = (int)belt[bt][DIRECTION];

	// 上キー
	if (mySpecialValue & (1 << 0))
	{
		y += friction[fric][4];
		if (collision())y -= friction[fric][4];
		//ここを変更する
		if (Y*L < y - MARGIN)y -= friction[fric][4];
	}

	// 左キー
	if (mySpecialValue & (1 << 1))
	{
		x -= friction[fric][4];
		if (collision())x += friction[fric][4];
		//ここを変更する
		if (0 * L > x + MARGIN)x += friction[fric][4];
	}

	// 右キー
	if (mySpecialValue & (1 << 2))
	{
		x += friction[fric][4];
		if (collision())x -= friction[fric][4];
		//ここを変更する
		if ((X - 1)*L < x - MARGIN)x -= friction[fric][4];
	}

	// 下キー
	if (mySpecialValue & (1 << 3))
	{
		y -= friction[fric][4];
		if (collision())y += friction[fric][4];
		//ここを変更する
		if (0 * L > y + MARGIN)y += friction[fric][4];
	}

	// 重力
	if (z > 0)
	{
		v -= 0.01;
		z += v;
		if (collision() && z< 1)
		{
			z -= v;
			v = 0;
		}
		if (z <= 0)
		{
			z = 0;
			v = 0;
		}
	}

	// ベルトコンベア
	if(bt_direction==X_DIR){
		x += belt[bt][SPEED];
		if (collision())x -= belt[bt][SPEED];
		//ここを変更する
		if ((X - 1)*L < x - MARGIN)x -= belt[bt][SPEED];
	}else if(bt_direction==Y_DIR){
		y += belt[bt][SPEED];
		if (collision())y -= belt[bt][SPEED];
		//ここを変更する
		if (Y*L < y - MARGIN)y -= belt[bt][SPEED];
	}
	if(y>Y-MARGIN) gameClear();

	//視点を移動
	glLoadIdentity();
	gluLookAt(0.0 + x, -10.0 + y, 2.0, 0.0 + x, 0.0 + y, 1.5, 0.0, 0.0, 1.0);

	glutTimerFunc(10, myTimerFunc, 0);

}

void myKeyboardFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		//ここを変更する
		v = 0.2;
		z += v;
		break;
	}
}

void mySpcialFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		mySpecialValue |= 1 << 0;//mySpecialValueの1bit目を1にする
		break;
	case GLUT_KEY_LEFT:
		mySpecialValue |= 1 << 1;//mySpecialValueの2bit目を1にする
		break;
	case GLUT_KEY_RIGHT:
		mySpecialValue |= 1 << 2;//mySpecialValueの3bit目を1にする
		break;
	case GLUT_KEY_DOWN:
		mySpecialValue |= 1 << 3;//mySpecialValueの4bit目を1にする
		break;
	default:
		break;
	}
}
void mySpcialUpFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		mySpecialValue &= ~(1 << 0);//mySpecialValueの1bit目を0にする
		break;
	case GLUT_KEY_LEFT:
		mySpecialValue &= ~(1 << 1);//mySpecialValueの2bit目を0にする
		break;
	case GLUT_KEY_RIGHT:
		mySpecialValue &= ~(1 << 2);//mySpecialValueの3bit目を0にする
		break;
	case GLUT_KEY_DOWN:
		mySpecialValue &= ~(1 << 3);//mySpecialValueの4bit目を0にする
		break;
	default:
		break;
	}

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	drawGround();
	drawJiki();
	drawTeki();
	drawString();

	glPopMatrix();
	glutSwapBuffers();
	timeKeeper();
}



void idle(void)
{
	glutPostRedisplay();
}
void init(void)
{
	glutKeyboardFunc(myKeyboardFunc);
	glutSpecialFunc(mySpcialFunc);
	glutSpecialUpFunc(mySpcialUpFunc);

	glutIdleFunc(idle);
	
	myTimerFunc(0);

	
}

clock_t start,now;

void replay(){
	x = 0.0;
	y = 0.0;
}

void timeKeeper(){
	int passtime;
	now = clock();
	passtime = now-start;
	
	if(passtime > 400000){
		sprintf(message, "Time Up!");
		start = clock();
		replay();
	}
}

void gameover(){
	sprintf(message, "Game Over!");
	replay();
}

void gameClear() {
	sprintf(message, "Clear!");
}


/*
 * 文字の描画
 */
void drawString()
{
	// 平行投影にする
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT); // glRasterPos2fの準備(左下(0, 0), 右上(WINDOW_WIDTH, WINDOW_HEIGHT)と考えていいように設定)
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glRasterPos2f(10, WINDOW_HEIGHT-20); // 書き始める左下の座標を設定
	int m;
	for (m = 0; m < strlen(message) ; m++) { // 文字の書き出し
		glutBitmapCharacter(FONT, message[m]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char *argv[])
{	
	start = clock();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("iraira");
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutDisplayFunc(display);
	mapdisplay();
	init();
	
	glutMainLoop();
	return 0;
}
