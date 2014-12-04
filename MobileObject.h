#ifndef __MOBILEOBJECT_H_INCLUDED__   
#define __MOBILEOBJECT_H_INCLUDED__

#include <cmath>

class MobileObject
{
protected:
	float x, y, z;
	float xOffset, yOffset, zOffset;	// allow the object to move off of its center
	float directionAngle;	// angle the object is facing

	float distanceTraveled;
	
	float radius;	// approximate size from center. mainly applicable to birds
public:
	/*MobileObject();
	~MobileObject();*/
	float goneDistance; // Distance that says when the object has left its original position

	virtual void update(float a, float b, float c, float direction) = 0;
	virtual void draw() = 0;

	void moveToward2D(float xGoal, float yGoal, float zGoal, float moveAmount);
	float getY(){ return y; }
	float getX(){ return x; }
	float getZ(){ return z; }

	bool destroy;
	bool gone;
private:

};

//MobileObject::MobileObject()
//{
//}
//
//MobileObject::~MobileObject()
//{
//}

// handle 2D motion
void MobileObject::moveToward2D(float xGoal, float yGoal, float zGoal, float moveAmount)
{
	// Get the distance between the points (hypotenuse)
	double xDiff = xGoal - x, yDiff = yGoal - y, zDiff = zGoal - z;
	if ((abs(xDiff) <= moveAmount) && (abs(zDiff) <= moveAmount))
	{
		destroy = true;
		return;
	}
	double hypotenuse = sqrt((xDiff*xDiff) + (zDiff*zDiff));

	// Get angle at current point
	double angle = asin(zDiff / hypotenuse);

	z += sin(angle) * moveAmount;
	if (xDiff > 0)
		x += cos(angle) * moveAmount;
	else
		x -= cos(angle) * moveAmount;

	distanceTraveled += moveAmount;		// keep track of how far it's moved

	if (distanceTraveled > goneDistance)	// mark if the object has completely left its original position
		gone = true;
}

#endif