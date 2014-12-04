#ifndef __WAVESET_H_INCLUDE__
#define __WAVESET_H_INCLUDE__

#include <algorithm> // used to copy the array
#include <ctime> // used for randomness

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class WaveSet 
{
	float x, y, z;
	float leftEdge, rightEdge;
	float zClose, zFar;
	float width, length;
	float xCenter, zCenter;
public:
	float originalHeight;	// reference height for wave. doesn't change
	float speed;	// how fast the wave section goes up & down
	float waveHeight;
	bool goDown, debug, destroy, gone;
	float distanceTraveled, goneDistance;

	WaveSet();
	WaveSet(float a, float b, float c, float left, float width, float close, float length, int height);
	//~WaveSet();

	void moveToward2D(float xGoal, float yGoal, float zGoal, float moveAmount);

	void update(float a, float b, float c, float direction){ ; }
	void draw();

	float getXcenter(){ return xCenter; }
	float getY(){ return y; }
	float getZcenter(){ return zCenter; }

private:

};




WaveSet::WaveSet()
{
	//srand(time(0));
	originalHeight = rand() % 20;
	if (originalHeight < 10)
		originalHeight *= 2;
	waveHeight = originalHeight;

	goDown = true;
	debug = false;

	leftEdge = -3.0;
	rightEdge = -leftEdge;
	zClose = 5.0;
	zFar = -zClose;

	// calculate center coordinates
	xCenter = (rightEdge - leftEdge) / 2;
	zCenter = (zClose + zFar) / 2;

	// Set position
	x = y = z = 0;

	speed = originalHeight / (originalHeight + 5.0);	// set wave speed

	gone = false;
	destroy = false;
	distanceTraveled = 0;
	if (length > width)
		goneDistance = length;
	else
		goneDistance = width;
}

WaveSet::WaveSet(float a, float b, float c, float left, float width, float close, float length, int height)
{
	originalHeight = rand() % height;
	if (originalHeight < 10)
		originalHeight *= 2;
	waveHeight = originalHeight;

	goDown = true;
	debug = false;

	leftEdge = left;
	rightEdge = leftEdge + width;
	zClose = close;
	zFar = zClose - length;

	this->width = width;
	this->length = length;

	// calculate center coordinates 
	xCenter = (rightEdge + leftEdge) / 2;
	zCenter = (zClose + zFar) / 2;

	// Set position
	x = a;	y = b;	z = c;

	speed = originalHeight / (originalHeight + 5.0);	// set wave speed

	gone = false;
	destroy = false;
	distanceTraveled = 0;
	if (length > width)
		goneDistance = length;
	else
		goneDistance = width;
}

void WaveSet::moveToward2D(float xGoal, float yGoal, float zGoal, float moveAmount)
{
	// Get the distance between the points (hypotenuse)
	double xDiff = xGoal - xCenter, yDiff = yGoal - y, zDiff = zGoal - zCenter;
	if ((abs(xDiff) <= width) && (abs(zDiff) <= length))
	{
		destroy = true;
		return;
	}
	double hypotenuse = sqrt((xDiff*xDiff) + (zDiff*zDiff));

	// Get angle at current point
	double angle = asin(zDiff / hypotenuse);

	zCenter += sin(angle) * moveAmount;
	if (xDiff > 0)
		xCenter += cos(angle) * moveAmount;
	else
		xCenter -= cos(angle) * moveAmount;

	distanceTraveled += moveAmount;		// keep track of how far it's moved

	if (distanceTraveled > goneDistance)	// mark if the object has completely left its original position
		gone = true;
}

void WaveSet::draw()
{
	// Update boundaries based on center coordinates
	leftEdge = xCenter - (width / 2);
	rightEdge = xCenter + (width / 2);
	zClose = zCenter + (length / 2);
	zFar = zCenter - (length / 2);
	// The constructor can be simplified due to this

	float controlPoints[6][4][3] =
	{
		{ { leftEdge, y, zClose }, { leftEdge + 2.75, y, zClose }, { rightEdge - 2.75, y, zClose }, { rightEdge, y, zClose } },
		{ { leftEdge, y, zClose * 0.6 }, { leftEdge + 2.75, waveHeight, zClose * 0.6 }, { rightEdge - 2.75, y, zClose * 0.6 }, { rightEdge, y, zClose * 0.6 } },
		{ { leftEdge, y, zClose * 0.2 }, { leftEdge + 2.75, y, zClose * 0.2 }, { rightEdge - 2.75, y, zClose * 0.2 }, { rightEdge, y, zClose * 0.2 } },
		{ { leftEdge, y, zFar * 0.2 }, { leftEdge + 2.75, y, zFar * 0.2 }, { rightEdge - 2.75, y, zFar * 0.2 }, { rightEdge, y, zFar * 0.2 } },
		{ { leftEdge, y, zFar * 0.6 }, { leftEdge + 2.75, y, zFar * 0.6 }, { rightEdge - 2.75, -waveHeight, zFar * 0.6 }, { rightEdge, y, zFar * 0.6 } },
		{ { leftEdge, y, zFar }, { leftEdge + 2.75, y, zFar }, { rightEdge - 2.75, y, zFar }, { rightEdge, y, zFar } },
	};


	if (debug == true)	// Draw Extras
	{
		glPointSize(5.0);

		// Draw green control points.
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 4; j++)
				glVertex3fv(controlPoints[i][j]);
		glEnd();

		// Draw the control polyhedron in light gray.
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.7, 0.7, 0.7);
		for (int i = 0; i < 5; i++)
		{
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < 4; j++)
			{
				glVertex3fv(controlPoints[i][j]);
				glVertex3fv(controlPoints[i + 1][j]);
			}
			glEnd();
		}
	}

	// Draw the wave itself ************************************************

	// Specify and enable this specific Bezier surface.
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPoints[0][0]);
	glEnable(GL_MAP2_VERTEX_3);

	glEnable(GL_AUTO_NORMAL);	// automatic calculation of normals

	glEnable(GL_CULL_FACE);	// culling
	glCullFace(GL_BACK);	// culling

	glPushMatrix();
	// Move to correct position
	glTranslatef(leftEdge, y, zClose);

	// Draw the Bezier surface using a mesh approximation.
	glColor3f(0.0, 0.0, 1.0);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);	// smooth surface

	glPopMatrix();

	glDisable(GL_CULL_FACE);	// end culling
}

//WaveSet::~WaveSet()
//{
//	;
//}

#endif