#ifndef __CLOUD_H_INCLUDE__
#define __CLOUD_H_INCLUDE__

#include "MobileObject.h"
#include "Character.h"
#include "Droplet.h"
#include <vector>
#include <cmath>
#include <time.h>
#include <cstdlib>


#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class particle
{
public:
	float r, g, b;
	float x, y, z;
	float radius;	// will revolve in its own little bubble
	float size;
	float angle;	// used in drawing the particle

	particle()
	{ 
		r = g = b = 0.5; 
		x = y = z = 0;
		radius = 1.0;
		size = 1.0;
		angle = 0.0;
	}

	particle(	float R, float G, float B, 
				float X, float Y, float Z,
				float rad, float pSize)
	{
		r = R;	g = G;	b = B;	x = X;	y = Y;	z = Z;	radius = rad;	size = pSize;	angle = 0.0;
	}

	void gyrate();	// move around in its bubble 

	//~particle(){ ; }
};


class Cloud : public MobileObject
{
	int particleCount;
	float r, g, b;
	float particleSize;
	float rotationSpeed;	// angle for rotation
	bool hit;
	std::vector<particle*> particles;
public:
	float xGoal, yGoal;

	Cloud();
	Cloud(	float R, float G, float B,
			float X, float Y, float Z,
			float rad, float count, float pSize);
	~Cloud();

	void draw();
	void update(float a, float b, float c, float direction){ ; }
	int reactToDroplet(Droplet *drop, Character &cha, int &globalScore);
	bool containsPoint(float X, float Y, float Z);
};


void particle::gyrate()
{
	srand(time(0));
	//glutWireCube(10.0);
	glPointSize(size);
	glColor3f(1.0, 1.0, 1.0);

	if (angle < 360.0)
	{
		// Rotate inside the revolution of one circle.. spiral effect
		glPushMatrix();
		//glTranslatef(x + radius * cos(angle), y + radius * sin(angle), 0.0);
		//glRotatef(5.0, 0.0, 0.0, 1.0);
		//glTranslatef(0, 15, 0);
		//glutWireSphere(size, 20, 20);

		glBegin(GL_POINTS);
		glVertex3f(x + radius * cos(angle), y + radius * sin(angle), 0.0);
		glEnd();

		if (rand() % 2 == 0)
			angle += 7.0;
		if (rand() % 6 == 3)
			angle += 5.0;
		if (rand() % 3 == 1)
			angle += 6.0;
		else
			angle += 4.0;

		angle += 7.0;

		glPopMatrix();
	}
	if (angle >= 360.0)
		angle = 0.0;
}

Cloud::Cloud()
{
	particleCount = 0;
	r = g = b = x = y = z = 0.0;
	hit = false;
}

Cloud::Cloud(float R, float G, float B,
	float X, float Y, float Z,
	float rad, float count, float pSize)
{
	srand(time(0));
	r = R;	g = G;	b = B;
	x = X;	y = Y;	z = Z;
	radius = rad;	particleCount = count;	particleSize = pSize;

	// increment by dividing range by the cubed root of the particle count due to the 3 loops
	double incrementAmt = (2 * radius) / std::cbrt(particleCount);

	// Generate even distribution
	for (double xx = -0; xx < radius; xx += particleSize)
	{
		for (double yy = -0; yy < radius; yy += particleSize)
		{
			for (double zz = -radius; zz < radius; zz += particleSize)
				particles.push_back(new particle(r, g, b, xx, yy, zz, particleSize * 3, particleSize));
		}
	}

	xGoal = -(rand() % 8 + x) * 1.5;
	yGoal = rand() % 9 + y;

	if (rand() % 7 == 3)
		rotationSpeed = 2;
	else if (rand() % 8 == 6)
		rotationSpeed = 7;
	else if (rand() % 2 == 0)
		rotationSpeed = 10;
	else
		rotationSpeed = 1;

	hit = false;
}

Cloud::~Cloud()
{
	for (int i = 0; i < particles.size(); i++)
		delete particles[i];
	particles.clear();
}

void Cloud::draw()
{
	glPushMatrix();

	glTranslatef(x, y, z);


	std::vector<particle*>::iterator pI = particles.begin();
	while (pI != particles.end())
	{
		glRotatef(rotationSpeed, 1.0, 1.0, 1.0);
		particle* part = *pI;
		part->gyrate();	// draw
		pI++;
	}


	glPopMatrix();
}

// Check if a droplet hit the cloud. Communicate once the droplet does hit
int Cloud::reactToDroplet(Droplet *drop, Character &cha, int &globalScore)
{
	if (containsPoint(drop->getX(), drop->getY(), drop->getZ()))
	{
		hit = true;
		destroy = true;

		if (!drop->hitSuccess)
		{
			cha.incrementScore();
			globalScore++;
			drop->hitSuccess = true;
		}
		return 1;
	}

	return -1;
}

bool Cloud::containsPoint(float X, float Y, float Z)
{
	float xDiff = X - x, yDiff = Y - y, zDiff = Z - z;

	float distance = sqrt((xDiff*xDiff)+(yDiff*yDiff)+(zDiff*zDiff));

	if (distance <= radius)
		return true;

	return false;
}


#endif