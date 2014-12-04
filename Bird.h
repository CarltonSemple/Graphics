#ifndef __BIRD_H_INCLUDE__   
#define __BIRD_H_INCLUDE__ 

#include "MobileObject.h"

#include <ctime>
#include <algorithm>
//#include <stdlib.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class Bird : public MobileObject
{
	float wingAngle;	// this will go between -90 & 90
	int wingDirection; // says if the wing is going down or up
	float wingSpan;
	float tipHeight;	// use as an alternative to wingAngle. affects the vertical positio of the wing tip
	float tipBoundary;
	float r, g, blue;	// color
public:
	Bird();
	Bird(float a, float b, float c, float di, float size);
	//~Bird();

	void update(float X, float Y, float Z, float Direction);
	void draw();

	void updateWings(float incrementAmount);
private:
	void drawWing(int wing);
	
};



Bird::Bird()
{
	srand(time(0));
	wingAngle = rand() % 90;	// start them off in random positions

	// randomly set wing direction
	if ((int)wingAngle % 2 == 0)
		wingDirection = -1;
	else
		wingDirection = 1;

	directionAngle = 0.0;
	wingSpan = rand() % 5;	// random wing length
	x = 0.0; y = 0.0; z = 0.0;
	gone = false;
	distanceTraveled = 0;

	destroy = false;
	// size of bird
	radius = 0.5;

	goneDistance = radius * 6;
}

Bird::Bird(float a, float b, float c, float di, float size)
{
	srand(time(0));
	wingAngle = rand() % 90;	// start them off in random positions

	// randomly set wing direction
	if ((int)wingAngle % 2 == 0)
		wingDirection = -1;
	else
		wingDirection = 1;

	directionAngle = di;
	wingSpan = rand() % 3;	// random wing length
	int temp = 3 * size;
	tipHeight = rand() % temp;
	if ((int)tipHeight % 2 == 0)
		tipHeight = -tipHeight;
	tipBoundary = size;

	x = a;
	y = b;
	z = c;
	gone = false;
	distanceTraveled = 0;

	destroy = false;
	// size of bird
	radius = size;

	goneDistance = radius * 6;
	r = (rand() % 7) / 10.0;
	if (r < 0.2)
		r = 0.4;	
	g = (rand() % 6) / 10.0;
	if (g < 0.2)
		g = 0.4;	
	blue = (rand() % 7) / 10.0;
	if (blue < 0.2)
		blue = 0.4;
	
}

void Bird::update(float X, float Y, float Z, float Direction)
{
	x = X;
	y = Y;
	z = Z;
	directionAngle = Direction;
}

void Bird::draw()
{
	GLUquadric *quadd = gluNewQuadric();

	// Draw body
	glPushMatrix();

	srand(time(0));

	glTranslatef(x, y, z);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glColor3f(1.0 - r, 1.0 - g, 1.0 - blue);
	glColor3f(r, g, blue);

	// Cull the back faces of the sphere.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	gluSphere(quadd, radius, 20, 20);
	glDisable(GL_CULL_FACE);

	// draw wings
	drawWing(0);
	drawWing(1);

	glPopMatrix();
}

//Bird::~Bird()
//{
//}

void Bird::drawWing(int wing)
{
	float zOffset = 0.5 * radius;
	float constant = 1;
	switch (wing)
	{
	case 0:
		break;
	case 1:
		constant = -constant;
		break;
	default:
		return;
		break;
	}
	glBegin(GL_POLYGON);
	glVertex3f(constant*radius, 0.0, zOffset);	// front hinge
	glVertex3f(constant*(radius + wingSpan), tipHeight, zOffset);	// front tip
	glVertex3f(constant*(radius + wingSpan), tipHeight, -zOffset);	// rear tip
	glVertex3f(constant*radius, 0.0, -zOffset);	// rear hinge
	glVertex3f(constant*radius, 0.0, zOffset);	// front hinge
	glEnd();
}

void Bird::updateWings(float incrementAmount)
{
	if (rand() % 2 == 0)
		incrementAmount = incrementAmount * 0.8;

	switch (wingDirection)
	{
	case -1:
		if (tipHeight - incrementAmount >= -tipBoundary)
			tipHeight -= incrementAmount;
		else
		{
			wingDirection = 1;
			tipHeight = -tipBoundary;
		}
		break;
	case 1:
		if (tipHeight + incrementAmount <= tipBoundary)
			tipHeight += incrementAmount;
		else
		{
			wingDirection = -1;
			tipHeight = tipBoundary;
		}
		break;
	default:
		break;
	}
}

#endif 