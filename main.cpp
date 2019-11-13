#include "header.h"

void Reshape(GLint w, GLint h)
{
	//изменить размеры окна
	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,width,height);
	gluOrtho2D(0.0,width,0.0,height);
}
void Process_Mouse_Move(int x, int y)
{
	//положение курсора
	p = point((GLint)x, height - (GLint)y);

	//определить существование активной точки
	LightPoint(p);

	//если разрешено перемещение
	if(moving)
	{
		//расчитать сдвиг
		Shift = point(Shift.x + Shift2.x - p.x, Shift.y + Shift2.y - p.y);
		Shift2 = p;
	}

	//если разрешено редактирование точек
	if(movingPoint)
	{
		//изменить точку
		ChangePoint(p);
	}
	glutPostRedisplay();
}
void Process_Mouse(int button, int state, int x, int y)
{
	if (state != GLUT_DOWN) return;

	//включение/выключение перемещения с помощью мыши
	if(button == GLUT_RIGHT_BUTTON)
	{
		Shift2 = point((GLint)x, height - (GLint)y);
		moving = !moving;
	}
	//добавление новой точки
	if(button == GLUT_LEFT_BUTTON)
	{
		curP = point((GLint)x, height - (GLint)y);
		AddPoint(curP);
	}
	//включение выключение редактирования точек
	if(button == GLUT_MIDDLE_BUTTON)
	{
		movingPoint = !movingPoint;
	}
}
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//нарисовать сетку
	glPushMatrix();
		glTranslatef((GLfloat)Shift.x, (GLfloat)Shift.y, 0);
		DrawMesh();	
	glPopMatrix();

	//нарисовать ломаную
	DrawScrap();
	
	//нарисовать сплайн
	DrawNURBSspline();

	//нарисовать координатные оси
	glPushMatrix();
		glTranslatef((GLfloat)Shift.x, (GLfloat)Shift.y, 0);
		DrawAxis();	
	glPopMatrix();
	
	//печать порядка базисных функций
	PrintM();
	
	//печатать координаты положения курсора
	PrintCoord(p);	
	glutSwapBuffers();
}
void Process_Normal_Keys(unsigned char key, int x, int y)
{
	//увеличить масштаб
	if(key == '+')
		if(zoom - 0.25 > 0)
			zoom -= 0.25;
	//уменьшить масштаб
	if(key == '-')
		zoom += 0.25;
	//увеличить порядок базисных функций
	if(key == '8')
	{
		if(m < 25)
		{
			m++;
			//пересчитать сплайн
			NURBSspline();
		}
	}
	//уменьшить порядок базисных функций
	if(key == '2')
	{
		if(m > 1)
		{
			m--;
			//пересчитать сплайн
			NURBSspline();
		}
	}
	//отображение весов контрольных точек
	if(key == 'w')
		visibleWeight = !visibleWeight;
	//увеличение веса активной точки
	if(key == '7')
		if(activePoint != 0 && activePoint != -1)
		{
			vecRealPoint[activePoint].weight += 0.5;
			//пересчитать сплайн
			NURBSspline();
		}
	//уменьшение веса активной точки
	if(key == '1')
		if(activePoint != 0 && activePoint != -1)
		{
			vecRealPoint[activePoint].weight -= 0.5;
			//пересчитать сплайн
			NURBSspline();
		}

	glutPostRedisplay();
}
void Process_Special_Keys(int key, int x, int y)
{
	int localShift = 3;
	if(key == GLUT_KEY_UP)
		Shift.y -= localShift;
	if(key == GLUT_KEY_DOWN)
		Shift.y += localShift;
	if(key == GLUT_KEY_LEFT)
		Shift.x += localShift;
	if(key == GLUT_KEY_RIGHT)
		Shift.x -= localShift;
	glutPostRedisplay();
}
void Initialize()
{
	glClearColor(1.0, 1.0, 1.0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,width,height);
	gluOrtho2D(0.0,width,0.0,height);

	//загрузка текстуры
	loadTexture();
}
void main(int argc, char *argv[])
{
	width = 800;
	height = 500;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("CG_RGR NURBS");
	Initialize();								//начальные установки

	glutKeyboardFunc(Process_Normal_Keys);		//обработка клавиш с кодами ascii
	glutSpecialFunc(Process_Special_Keys);		//обработка не-ascii клавиш
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutMouseFunc(Process_Mouse);
	glutPassiveMotionFunc(Process_Mouse_Move);
	glutMainLoop();
}
