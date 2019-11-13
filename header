#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <glut.h> 
#include <glaux.h>
#include <vector>
#include <gl\gl.h>
#include <iostream>
#include <sstream>

#pragma comment(lib,"glaux.lib")
using namespace std;

GLint width, height;		//ширина и высота окна
GLfloat zoom = 1;			//коэффициент масштабирования изображения
int ind = 60;				//расстояние между координатными подписями
bool moving = false;		//перемещение с помощью мыши
bool movingPoint = false;	//перемещение точки с помощью мыши
bool visibleWeight = true;	//показ весов вершин
int m = 3;					//порядок базисных ф-й
int n = 0;					//число контрольных узлов
int activePoint = -1;		//номер активной точки

//класс точки в экранных координатах
class point	
{
public:
	GLint x, y;
	point()
	{
	}
	point(GLint _x, GLint _y)
	{
		x = _x;
		y = _y;
	}
};

point p;					//позиция мыши
point Shift = point(0, 0);	//общий сдвиг изображения
point Shift2;	//сдвиг для движения
point curP;		//точка на добаление

//класс точки в реальных координатах
class realPoint
{
public:
	double x, y;	//координаты точки
	double weight;	//вес точки
	realPoint()
	{
	}
	realPoint(double _x, double _y)
	{
		x = _x;
		y = _y;
		weight = 1.;
	}
	//преобразование экранных координат к реальным
	realPoint(point _point)
	{
		x = _point.x;
		y = _point.y;

		//преобразование экранных координат к реальным
		//учет сдвига
		x -= Shift.x;
		y -= Shift.y;
		//учет положения центра координат
		x = x - width/2.;
		y = y - height/2.;
		//учет числа пикселей на одно деление
		x /= double(ind);
		y /= double(ind);
		//учет масштабирования
		x *= zoom;
		y *= zoom;

		weight = 1.;
	}
	point getPoint();
};

//преобразование реальных координат к экранным
point realPoint::getPoint()
{
	double _x = x;
	double _y = y;

	//учет масштабирования
	_x /= zoom;
	_y /= zoom;
	//учет числа пикселей на одно деление
	_x *= double(ind);
	_y *= double(ind);
	//учет положения центра координат
	_x = _x + width/2.;
	_y = _y + height/2.;
	//учет сдвига
	_x += Shift.x;
	_y += Shift.y;

	return point(_x, _y);
}

//vector <point> vecPoint;
vector <realPoint> vecRealPoint;	//вектор контрольных точек
vector <double> vecParameter;		//вектор параметров
vector <realPoint> NURBS;			//вектор точек для построения сплайна

unsigned int names_tex[10];//имена текстур
//загрузка текстуры
void loadTexture()
{
	//загрузить текстуру
	AUX_RGBImageRec *image1 = auxDIBImageLoad(L"1.bmp");
	glGenTextures(10, names_tex); //создание имен текстур

	glBindTexture (GL_TEXTURE_2D, names_tex[0]); //выбрать текущую текстуру
	gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
}
//отрисовка сетки
void DrawMesh()
{
	glBindTexture(GL_TEXTURE_2D, names_tex[0]);	//выбрать текстуру
	glEnable(GL_TEXTURE_2D);					//разрешить текстурирование
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//разрешить повтор фигуры 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	//по обоим координатам
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);		//не учитывать цвет объекта
	glBegin(GL_QUADS);
		glTexCoord2d(0,0);		glVertex2d(-5000,-5000);
		glTexCoord2d(0,50);		glVertex2d(-5000, 5000);
		glTexCoord2d(50,50);	glVertex2d( 5000, 5000);
		glTexCoord2d(50,0);		glVertex2d( 5000,-5000);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
//отрисовка координатных осей
void DrawAxis()
{
	int len = 5;			//длина штриха
	int cenOx = width/2.0;	//центр оси по Ох
	int cenOy = height/2.0;	//центр оси по Оу
	int lineShiftX = 14;	//сдвиг подписи с Ох
	int sepShiftX = 2;		//сдвиг подписи от линии разделения
	int lineShiftY = 7;		//сдвиг подписи с Оу
	int sepShiftY = 3;		//сдвиг подписи от линии разделения

	//нарисовать координатные подписи
	for(int i = 0; i < 100; i++)
	{
		glLineWidth(1);
		glColor3ub(255, 0, 0);
		//по Ох
		glBegin(GL_LINES);
			glVertex2i(cenOx + i*ind, cenOy - len);
			glVertex2i(cenOx + i*ind, cenOy + len);
		glEnd();
		glBegin(GL_LINES);
			glVertex2i(cenOx - i*ind, cenOy - len);
			glVertex2i(cenOx - i*ind, cenOy + len);
		glEnd();

		//по Оу
		glBegin(GL_LINES);
			glVertex2i(cenOx - len, cenOy + i*ind);
			glVertex2i(cenOx + len, cenOy + i*ind);
		glEnd();
		glBegin(GL_LINES);
			glVertex2i(cenOx - len, cenOy - i*ind);
			glVertex2i(cenOx + len, cenOy - i*ind);
		glEnd();


		//вывести подписи к координатным осям
		glColor3ub(0, 0, 0);
		char dest[10];
		double num = (double)i * zoom;

		//преобразование числа в строку
		sprintf(dest, "%0.2lf", num);
		string out = string(dest);
		
		//по оси Ох
		glRasterPos2i(cenOx + i*ind + sepShiftX, cenOy - lineShiftX);
		for(auto a : out) 
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
		glRasterPos2i(cenOx - i*ind + sepShiftX, cenOy - lineShiftX);
		for(auto a : out) 
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);

		//по оси Оу
		if(i != 0)
		{
			glRasterPos2i(cenOx + lineShiftY, cenOy + i*ind - sepShiftY);
			for(auto a : out) 
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
			glRasterPos2i(cenOx + lineShiftY, cenOy - i*ind - sepShiftY);
			for(auto a : out) 
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
		}
	}

	//нарисовать координатные оси
	glLineWidth(2);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
		glVertex2i(-5000, cenOy);
		glVertex2i( 5000, cenOy);
	glEnd();
	glBegin(GL_LINES);
		glVertex2i(cenOx,-5000);
		glVertex2i(cenOx, 5000);
	glEnd();
}
//печать координат положения курсора
void PrintCoord(point pos)
{
	//вывести подписи к положению курсора
	char destX[10];
	char destY[10];
	realPoint rlPt = realPoint(pos);

	//преобразование числа в строку
	sprintf(destX, "%0.2lf", rlPt.x);
	sprintf(destY, "%0.2lf", rlPt.y);
	string outX = "x = " + string(destX);
	string outY = "y = " + string(destY);

	//вывод координат
	glColor3ub(0, 0, 0);
	glRasterPos2i(pos.x + 12, pos.y + 2);
	for(auto a : outX) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
	glRasterPos2i(pos.x + 12, pos.y - 10);
	for(auto a : outY) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
	glutPostRedisplay();
}
//печать порядка базисных функций
void PrintM()
{
	//печать порядка базисных функций
	char dest[10];
	sprintf(dest, "%i", m);
	string out = "m = " + string(dest);
	glRasterPos2i(10, height - 10);
	for(auto a : out) 
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, a);
}
//формирование вектора параметров(true - если вектор параметров сформирован;
//								  false - если недостаточно узлов для построения)
bool SetParameters()
{
	//если число контрольных точек меньше требуемого строить нечего
	if(n < m + 1)
		return false;

	//очистить вектор параметров
	vecParameter.clear();

	//сформировать вектор параметров
	for(int i = 0; i < m+1; i++)
		vecParameter.push_back(0);
	for(int i = 0; i < n-(m+1); i++)
		vecParameter.push_back(i+1);
	for(int i = n-m; i < n+1; i++)
		vecParameter.push_back(n-m);
}
//вычисление базисной функции
double OneBasicFunc(int i, double u)
{
	double N[30];
	int p = vecParameter.size() - 1;
	if((i == 0 && u == vecParameter[0]) ||
		(i == p - m - 1 && u == vecParameter[p]))
		return 1.;

	if(u < vecParameter[i] || u >= vecParameter[i+m+1])
		return 0.;

	for(int j = 0; j <= m; j++)
		if(u >= vecParameter[i+j] && u < vecParameter[i+j+1])
			N[j] = 1.;
		else 
			N[j] = 0.;

	for(int k = 1; k <= m; k++)
	{
		double saved;
		if(N[0] == 0.)
			saved = 0.;
		else
			saved = ((u - vecParameter[i])*N[0])/(vecParameter[i+k]-vecParameter[i]);

		for(int j = 0; j < m - k + 1; j++)
		{
			double Uleft = vecParameter[i+j+1];
			double Uright = vecParameter[i+j+k+1];
			if(N[j+1] == 0.)
			{
				N[j] = saved;
				saved = 0.;
			}
			else
			{
				double temp = N[j+1] / (Uright - Uleft);
				N[j] = saved + (Uright-u)*temp;
				saved = (u-Uleft)*temp;
			}
		}
	}
	return N[0];
}
//построение NURBS сплайна
void NURBSspline()
{
	//если сформировано расширенное множество узлов
	if(SetParameters())
	{
		//очистить предыдущий набор значений
		NURBS.clear();

		//число точек в построении сплайна
		int num = n*20;

		//шаг по вектору параметров
		double h = abs(vecParameter[0] - vecParameter[n+m-1])/(double)num;
		//вычислить координаты сплайна
		for(int k = 0; k < num+1; k++)
		{
			double u = vecParameter[0] + k*h;
			double x = 0.;		
			double y = 0.;
			double w = 0.;
			if(k == num)
				u = vecParameter[vecParameter.size()-1];

			//вычисление координат сплайна
			for(int i = 0; i < n; i++)
			{
				//вычисление базовой функции
				double Nim = OneBasicFunc(i, u);
				w += vecRealPoint[i].weight * Nim;
				x += vecRealPoint[i].weight * vecRealPoint[i].x * Nim;
				y += vecRealPoint[i].weight * vecRealPoint[i].y * Nim;
			}
			NURBS.push_back(realPoint(x/w, y/w));
		}
	}
}
//отрисовка NURBS сплайна
void DrawNURBSspline()
{
	point pt;
	
	//Отрисовка ломаной
	glColor3ub(255, 0, 0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < NURBS.size(); i++)
	{
		//преобразовать координаты реальной точки к экранным
		pt = NURBS[i].getPoint();
		//отрисовать
		glVertex2i(pt.x, pt.y);
	}
	glEnd();
}
//добаление точки в вектор
void AddPoint(point _point)
{
	//добавить в вектор координаты реальной точки
	vecRealPoint.push_back(realPoint(_point));

	//увеличить число вершин
	n++;

	//пересчитать сплайн
	NURBSspline();
}
//отрисовка контрольной ломаной
void DrawScrap()
{
	point pt;
	
	//Отрисовка ломаной
	glColor3ub(226, 192, 226);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < vecRealPoint.size(); i++)
	{
		//преобразовать координаты реальной точки к экранным
		pt = vecRealPoint[i].getPoint();
		//отрисовать
		glVertex2i(pt.x, pt.y);
	}
	glEnd();

	//Отрисовка контрольных точек
	for(int i = 0; i < vecRealPoint.size(); i++)
	{
		glPointSize(5);
		glColor3ub(98, 0, 226);
		glBegin(GL_POINTS);
		//преобразовать координаты реальной точки к экранным
		pt = vecRealPoint[i].getPoint();
		//отрисовать
		glVertex2i(pt.x, pt.y);
		glEnd();

		//напечатать вес точки
		if(visibleWeight)
		{
			char dest[10];
			sprintf(dest, "%0.2lf", vecRealPoint[i].weight);
			string out = "w = " + string(dest);
			glColor3ub(0, 0, 0);
			glRasterPos2i(pt.x - 10, pt.y + 5);
			for(auto a : out) 
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, a);
		}
	}

	//если существует активная точка
	//выделить ее другим цветом
	if(activePoint != -1)
	{
		glPointSize(6);
		glColor3ub(255, 201, 14);
		glBegin(GL_POINTS);
		//преобразовать координаты реальной точки к экранным
		pt = vecRealPoint[activePoint].getPoint();
		//отрисовать
		glVertex2i(pt.x, pt.y);
		glEnd();
	}
}
//выбрать активную точку
bool LightPoint(point pos)
{
	point ptCrd;
	int area = 7;
	for(int i = 0; i < vecRealPoint.size(); i++)
	{
		ptCrd = vecRealPoint[i].getPoint();
		//если координаты курсора попадают в область нахождения какой-нибудь точки
		if(pos.y < ptCrd.y + area && pos.y > ptCrd.y - area &&
			pos.x < ptCrd.x + area && pos.x > ptCrd.x - area)
		{
			activePoint = i; 
			return true;
		}
	}
	//активной точки нет
	activePoint = -1;
	return false;
}
//изменить координату точки
void ChangePoint(point _point)
{
	//если существует активная точка
	if(activePoint != -1)
	{
		//изменить координаты точки
		double weight = vecRealPoint[activePoint].weight;
		vecRealPoint[activePoint] = realPoint(_point);
		vecRealPoint[activePoint].weight = weight;
	}
	//пересчитать сплайн
	NURBSspline();
}
