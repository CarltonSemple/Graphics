#include "Button2D.h"
#include <ctime> // used for randomness
#include <algorithm>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

Button2D::Button2D()
{
	xCenter = 0.0; yCenter = 0.0; zCenter = 0.0;
}

// Create a Button from the given coordinates
Button2D::Button2D(float x, float y, float z,
					float width, float height,
					char *n)
{
	hWidth = width / 2;	hHeight = height / 2;
	xCenter = x;	yCenter = y;	zCenter = z;
	left = xCenter - hWidth;
	right = xCenter + hWidth;
	top = yCenter + hHeight;
	bottom = yCenter - hHeight;
	text = n;
}

// Create a Button From the Given Boundaries
Button2D::Button2D(float l, float r, float b, float t, char *n, float z, char action)
{
	left = l;	right = r;	bottom = b;	top = t;
	xCenter = (right + left) / 2;
	yCenter = (top + bottom) / 2;
	zCenter = z;
	hWidth = (right - left) / 2;
	hHeight = (top - bottom) / 2;
	text = n;
	this->action = action;
}

Button2D::~Button2D()
{
}

void Button2D::draw(bool blueGreen, float r, float g, float b)
{
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(xCenter - 0.2 * hWidth, yCenter, zCenter);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, text);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (blueGreen)
	{
		// Flip the green & blue values for this time
		if (green == 1)
			green = 0;
		else
			green = 1;
		if (blue == 1)
			blue = 0;
		else
			blue = 1;

		glColor3f(0.0, green, blue);
	}
	else
		glColor3f(r, g, b);

	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(left, bottom, zCenter);
		glVertex3f(left, top, zCenter);
		glVertex3f(right, top, zCenter);
		glVertex3f(right, bottom, zCenter);
		glVertex3f(left, bottom, zCenter);
	glEnd();	

}

void Button2D::updatePosition(float l, float r, float b, float t, float z)
{
	left = l;	right = r;	bottom = b;	top = t;
	xCenter = (right + left) / 2;
	yCenter = (top + bottom) / 2;
	zCenter = z;
	hWidth = (right - left) / 2;
	hHeight = (top - bottom) / 2;
}

bool Button2D::containsCoordinate(float x, float y)
{
	if (x >= left && x <= right)
	{
		if (y <= top && y >= bottom)
			return true;
	}

	return false;
}

void Button2D::incrementOrDecrement(int &affectedNumber)
{
	switch (action)
	{
	case 'i':
		affectedNumber++;
		break;
	case 'd':
		affectedNumber--;
		break;
	default:
		break;
	}
}

// Routine to draw a bitmap character string.
void Button2D::writeBitmapString(void *font, char *string)
{
	char *c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}