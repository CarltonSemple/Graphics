#include "Button3D.h"
#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

Button3D::Button3D()
{
	xCenter = yCenter = zCenter = xRotation = yRotation = zRotation = 0.0;
	direction = 'u';
	clicked = false;
}

Button3D::Button3D(float x, float y, float z, 
					float xR, float yR, float zR)
{
	xCenter = x;	yCenter = y;	zCenter = z;
	xRotation = xR;	yRotation = yR;	zRotation = zR;
	direction = 'd';	// down
	clicked = false;
}

Button3D::~Button3D()
{
}

void Button3D::draw(bool selectionModeEnabled, unsigned int closestName, int highlightFrames)
{
	float hWidth = 2.5, hHeight = 2.5, zOff = 0.5;

	glPushMatrix();

	glTranslatef(xCenter, yCenter, zCenter);	// translate before rotate

	glRotatef(xRotation, 1.0, 0.0, 0.0);
	glRotatef(yRotation, 0.0, 1.0, 0.0);
	glRotatef(zRotation, 0.0, 0.0, 1.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (selectionModeEnabled)
		glLoadName(1);	// name is 1 for this object
	if (highlightFrames > 0 && closestName == 1)
	{
		clicked = true;
	}

	glColor3f(1.0, 1.0, 1.0);
	//glutSolidCube(3.0);
	glBegin(GL_POLYGON);
		glVertex3f(-hWidth, hHeight, zOff);	// top left front
		glVertex3f(-hWidth, hHeight, -zOff);	// top left back
		glVertex3f(-hWidth, hHeight, zOff);	// top left front
		//glColor3f(0.0, 1.0, 1.0);
		glVertex3f(hWidth, hHeight, zOff);	// top right front
		glVertex3f(hWidth, hHeight, -zOff);	// top right back
		glVertex3f(hWidth, hHeight, zOff);	// top right front
		//glColor3f(1.0, 0.0, 1.0);
		glVertex3f(hWidth, -hHeight, zOff);	// bottom right front
		glVertex3f(hWidth, -hHeight, -zOff);	// bottom right back
		glVertex3f(hWidth, -hHeight, zOff);	// bottom right front
		//glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-hWidth, -hHeight, zOff);	// bottom left front
		glVertex3f(-hWidth, -hHeight, -zOff);	// bottom left back
		glVertex3f(-hWidth, -hHeight, zOff);	// bottom left front
		glVertex3f(-hWidth, hHeight, zOff);	// top left front
		// back
		//glColor3f(1.0, 1.0, 1.0);
		glVertex3f(-hWidth, hHeight, -zOff);	// top left back
		//glColor3f(0.0, 1.0, 1.0);
		glVertex3f(hWidth, hHeight, -zOff);	// top right back
		//glColor3f(1.0, 0.0, 1.0);
		glVertex3f(hWidth, -hHeight, -zOff);	// bottom right back
		//glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-hWidth, -hHeight, -zOff);	// bottom left back
		//glColor3f(1.0, 1.0, 1.0);
		glVertex3f(-hWidth, hHeight, -zOff);	// top left back

	glEnd();

	glEnable(GL_AUTO_NORMAL);

	glPopMatrix();

	// Clear name stack
	if (selectionModeEnabled)
		glPopName();
}

void Button3D::rotate(float increment)
{
	xRotation += increment;	yRotation += increment;	zRotation += increment;

	// bring the angles back to 0
	if (xRotation >= 360.0)
		xRotation = 0.0;
	if (yRotation >= 360.0)
		yRotation = 0.0;
	if (zRotation >= 360.0)
		zRotation = 0.0;
}

void Button3D::move(float xChange, float yChange, float zChange)
{
	xCenter += xChange;
	yCenter += yChange;
	zCenter += zChange;
}

void Button3D::verticalScroll(float yLow, float yHigh, float increment)
{
	if (yCenter >= yLow && yCenter <= yHigh)	// must be in the range
	{
		switch (direction)
		{
		case 'u':
			if (yCenter + increment > yHigh)
			{
				yCenter = yHigh;
				direction = 'd';
			}
			else
				yCenter += increment;
			break;
		case 'd':
			if (yCenter - increment < yLow)
			{
				yCenter = yLow;
				direction = 'u';	// switch direction
			}
			else
				yCenter -= increment;
			break;
		default:
			break;
		}

	}
}