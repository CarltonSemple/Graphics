#ifndef __DROPLET_H_INCLUDE__
#define __DROPLET_H_INCLUDE__

#include "MobileObject.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class Droplet : public MobileObject
{
public:
	bool letFly;	// signal to let the droplet free
	bool hitSuccess;

	Droplet();
	Droplet(float X, float Y, float Z, float Radius);
	~Droplet();

	void draw();
	void drawSuccess();
	bool containsPoint(float X, float Y, float Z);
	void update(float a, float b, float c, float direction){ x = a;	y = b;	z = c; }

private:
	void explode();
};



Droplet::Droplet()
{
	letFly = true;
	hitSuccess = false;
}

Droplet::Droplet(float X, float Y, float Z, float Radius)
{
	x = X;	y = Y;	z = Z;
	radius = Radius;
	distanceTraveled = 0.0;
	destroy = false;
	gone = false;
	letFly = false;
	hitSuccess = false;
}

Droplet::~Droplet()
{
	;
}

void Droplet::draw()
{
	glPushMatrix();

	glTranslatef(x, y, z);
	glColor3f(0.0, 0.0, 0.8);
	glutSolidSphere(radius, 20, 20);

	glPopMatrix();
}

void Droplet::drawSuccess()
{
	float multiplier = 2.0;
	glPushMatrix();

	glTranslatef(x, y, z);
	glColor3f(0.0, 0.0, 0.8);
	glutSolidSphere(radius, 20, 20);

	glPushMatrix();	// 1
	glTranslatef(-radius*multiplier, 0.0, 0.0);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();

	glPushMatrix();	// 2
	glTranslatef(radius*multiplier, 0.0, 0.0);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();

	glPushMatrix();	// 3
	glTranslatef(0.0, radius*multiplier, 0.0);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();

	glPushMatrix();	// 4
	glTranslatef(0.0, -radius*multiplier, 0.0);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();

	glPopMatrix();
}

bool Droplet::containsPoint(float X, float Y, float Z)
{

	return false;
}

void Droplet::explode()
{

}


#endif