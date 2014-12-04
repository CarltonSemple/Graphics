#ifndef __CHARACTER_H_INCLUDE__
#define __CHARACTER_H_INCLUDE__

#include "BitMap.h"
#include "Droplet.h"
#include <deque>
#include <cmath>
#include <string>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class Character
{
	float xCenter, yCenter, zCenter;
	float xGlider, yGlider, zGlider;	// The center of the glider, above the character

	float x_ch_EXCL, y_ch_EXCL, z_ch_EXCL;	// Posiiton to draw the character solo (no glider)
	float xRotate_ch_EXCL, yRotate_ch_EXCL, zRotate_ch_EXCL;	// Character solo rotation

	float bodyThickness, bodyHeight;
	float half_BH;	// introduced to make the code cleaner
	float headRadius;
	float torsoWidth, centerWidth;
	
	// Fun extras
	float headSeparation;	// separation of head from body
	float r_accent, g_accent, b_accent;	// accent color
	int score;

	// droplets
	float flyRange;
public:

	std::deque<Droplet*> wawaDrops;	// water droplets being held
	std::deque<Droplet*> goneDroplets;	// droplets that have been let free
private:
	float gWidth, half_GW;	// glider width

	float x_glider_EXCL, y_glider_EXCL, z_glider_EXCL;	// Position to draw the glider separately (exclusively)
	float xRotate_glider_EXCL, yRotate_glider_EXCL, zRotate_glider_EXCL;	// Glider Exclusive Rotation
	
public:
	BitMap *bmp;
	

	Character();
	Character(float X, float Y, float Z, float thickness, float torWidth, float height, float headSize, int dropCount, float dropletRange);
	~Character();

	//inline
	void draw();
	void drawForSelection(float x, float y, float z, float xRotation, float yRotation, float zRotation);	// Draw the character without the glider
	void updatePosition(float X, float Y, float Z);
	void updateAccentColor(float r, float g, float b);

	void moveAmount(float amount, float xChange, float yChange, float zChange);

	// Draw the Glider By Itself
	void drawGlider();
	void updateGliderPosition_Exclusive(float x, float y, float z, float xRotation, float yRotation, float zRotation);	// Update the glider exclusive position

	void spewWater();
	void drawDroplets();
	void moveDroplets(float moveAmount);	// move that droplets that are flying. Remove them when they hit or are too far

	// Score
	void setScore(int s){ score = s; }
	void incrementScore(){ score++; }
	int getScore(){ return score; }

private:	// private functions
	
	void hangGlider(bool alone);

	void head(bool solo);
	void torso(bool solo);
	void leg(int leg);
	void arm(int arm);
	void hand(int hand);
	void foot(int foot);
};



Character::Character()
{
	bmp = new BitMap();	// Allocate memory for the BitMap
	score = 0.0;
}

Character::Character(float X, float Y, float Z,
	float thickness, float torWidth, float height,
	float headSize,
	int dropCount, float dropletRange)
{
	// Body details
	bodyThickness = thickness;
	bodyHeight = height;	half_BH = bodyHeight;	headRadius = headSize;	torsoWidth = torWidth;

	headSeparation = headRadius;
	r_accent = 1.0;	g_accent = 1.0;	b_accent = 1.0;
	centerWidth = torsoWidth / 5;

	gWidth = 4 * bodyHeight;	// glider width
	half_GW = gWidth / 2;

	// Position
	this->xCenter = X;	this->yCenter = Y; this->zCenter = Z;
	xGlider = 0;
	yGlider = 3 * bodyThickness;
	zGlider = -half_BH;

	bmp = new BitMap();	// Allocate memory for the BitMap

	// Create water droplet arsenal
	for (int i = 0; i < dropCount; i++)
	{
		float dropRadius = 1.0;
		Droplet* nD = new Droplet(xCenter, yCenter, zCenter, dropRadius);
		nD->goneDistance = 2.5 * dropRadius;	// distance to conclude that the droplet has left its starting position
		flyRange = dropletRange;	
		wawaDrops.push_back(nD);
	}

	score = 0.0;
}

Character::~Character()
{
	delete bmp;	// delete the bitmap

	for (int i = 0; i < wawaDrops.size(); i++)
	delete wawaDrops[i];
	wawaDrops.clear();
}

void Character::draw()
{
	glPushMatrix();
	glTranslatef(xCenter, yCenter, zCenter);

		hangGlider(false);	// Draw the glider in relation to the character's position

		// Draw a connection between hang glider and player


		// Draw player body
		glPushMatrix();
		glTranslatef(0.0, -5.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glColor3f(1.0, 1.0, 1.0);
			head(true);
			torso(true);
			arm(0);
			hand(0);
			arm(1);
			hand(1);
			leg(0);
			foot(0);
			leg(1);
			foot(1);

		glPopMatrix();

	glPopMatrix();
}

// Draw the character without the glider
void Character::drawForSelection(float x, float y, float z,
	float xRotation, float yRotation, float zRotation)
{
	x_ch_EXCL = x;	y_ch_EXCL = y;	z_ch_EXCL = z;

	glPushMatrix();
	glTranslatef(x_ch_EXCL, y_ch_EXCL, z_ch_EXCL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0, 1.0, 1.0);

	head(true);
	torso(true);
	arm(0);
	hand(0);
	arm(1);
	hand(1);
	leg(0);
	foot(0);
	leg(1);
	foot(1);
	glEnable(GL_AUTO_NORMAL);


	glPopMatrix();
}

// Move the character and its droplets
void Character::updatePosition(float X, float Y, float Z)
{
	xCenter = X;	yCenter = Y; zCenter = Z;
	
	// update droplets
	std::deque<Droplet*>::iterator dI = wawaDrops.begin();
	while (dI != wawaDrops.end())
	{
		Droplet* drop = *dI;
		drop->update(xCenter, yCenter, drop->getZ(), 0);

		dI++;
	}
}


void Character::updateAccentColor(float r, float g, float b)
{

}

// Draw the Glider By Itself
void Character::drawGlider()
{
	hangGlider(true);	// Draw alone
}

// Update the glider exclusive position
void Character::updateGliderPosition_Exclusive(float x, float y, float z,
	float xRotation, float yRotation, float zRotation)
{
	x_glider_EXCL = x;	y_glider_EXCL = y;	z_glider_EXCL = z;
	xRotate_glider_EXCL = xRotation;
	yRotate_glider_EXCL = yRotation;
	zRotate_glider_EXCL = zRotation;
}


// Spawn Water and Let it Fly
void Character::spewWater()
{
	std::deque<Droplet*>::iterator dIt = wawaDrops.begin();
	if (dIt == wawaDrops.end())	// do nothing if there's no more water droplets
		return;

	Droplet* drop = *dIt;
	drop->letFly = true;	// signal to let it fly

	goneDroplets.push_back(*dIt);	// add to the droplets that have been released

	// remove from droplets still available
	wawaDrops.pop_front();
}

void Character::drawDroplets()
{
	// draw the droplets that have been released
	std::deque<Droplet*>::iterator dIt = goneDroplets.begin();
	while (dIt != goneDroplets.end())
	{
		Droplet* drop = *dIt;
		if (!drop->hitSuccess)
			drop->draw();

		dIt++;
	}
}

// Move droplets that are in the goneDroplets collection
void Character::moveDroplets(float moveAmount)
{
	std::deque<Droplet*>::iterator gIt = goneDroplets.begin();

	if (goneDroplets.size() == 0)
		return;
	// Remove droplets that are too far 
	while (gIt != goneDroplets.end())
	{
		Droplet* drop = *gIt;
		if (drop->destroy)
		{
			goneDroplets.pop_front();
			delete drop;

			if (goneDroplets.size() == 0)
				break;
			gIt = goneDroplets.begin();	// prevent error
		}
		else
			break;
	}

	// Move the droplets that are gone
	// Don't draw the ones that hit something
	gIt = goneDroplets.begin();
	while (gIt != goneDroplets.end())
	{
		Droplet *drop = *gIt;
		drop->moveToward2D(drop->getX(), drop->getY(), -200.0, moveAmount);

		gIt++;
	}


}


// Draw the hang glider, which is made in relation to the character (translation)
// This also has a texture
void Character::hangGlider(bool alone)
{
	glEnable(GL_TEXTURE_2D);	// Enable texturing
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	float wingDip = abs(0.1 * (yGlider));	// how low the wings go
	float xScale = 1.0, yScale = 1.0, zScale = 1.0;
	glPushMatrix();
	if (alone)	// Draw alone
	{
		glTranslatef(x_glider_EXCL, y_glider_EXCL, z_glider_EXCL);
		glRotatef(xRotate_glider_EXCL, 1.0, 0.0, 0.0);
		glRotatef(yRotate_glider_EXCL, 0.0, 1.0, 0.0);
		glRotatef(zRotate_glider_EXCL, 0.0, 0.0, 1.0);
		xScale = yScale = zScale = 14.0;	// Scale the glider
	}
	else	// Draw in relation to the character
		glTranslatef(xGlider, yGlider, zGlider);	// move away from the character's center

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_POLYGON);

	// Texture before each vertex, as found at: http://stackoverflow.com/questions/8259628/triangle-texture-mapping-opengl
	glColor3f(1.0, 0.0, 0.0);
	glTexCoord2f(0.5, 1);	// front tip
	glVertex3f(0.0, 0.0, zScale * -(half_BH + 0.2 * bodyHeight));
	glTexCoord2f(1, 0);		// right edge
	glVertex3f(half_GW, yScale * -wingDip, zScale * half_BH);
	glTexCoord2f(0.5, 0);	// back mid
	glVertex3f(0.0, 0.0, zScale * (half_BH / 2));
	glTexCoord2f(0, 0);		// left edge
	glVertex3f(xScale * -half_GW, yScale * -wingDip, zScale * half_BH);

	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);	// Disable texturing
}

/********************** Character Body Functions **********************/

void Character::head(bool solo)
{
	GLUquadric *quadd = gluNewQuadric();

	glPushMatrix();

	glTranslatef(0.0, half_BH + headSeparation, 0.0);
	gluSphere(quadd, headRadius, 20, 20);


	glPopMatrix();
}

void Character::torso(bool solo)
{
	GLUquadric *quadd = gluNewQuadric();

	float widthHalf = torsoWidth / 2;

	float zOffset = bodyThickness / 2;

	glPushMatrix();

	//gluCylinder(quadd, bodyThickness, bodyThickness, half_BH - headRadius, 20, 20);
	glBegin(GL_POLYGON);
	if (!solo)
	{

	}
	else // solo
	{
		glVertex3f(-widthHalf, half_BH - headRadius, zOffset);	// top left
		glVertex3f(widthHalf, half_BH - headRadius, zOffset);	// top right front
		glVertex3f(widthHalf, half_BH - headRadius, -zOffset);	// top right back
		glVertex3f(widthHalf, half_BH - headRadius, zOffset);	// top right front
		glVertex3f(centerWidth, 0.0, zOffset);	// bottom right front
		glVertex3f(centerWidth, 0.0, -zOffset);	// bottom right back
		glVertex3f(centerWidth, 0.0, zOffset);	// bottom right front
		glVertex3f(-centerWidth, 0.0, zOffset);	// bottom left of torso. at body's center (y-axis)
		glVertex3f(-centerWidth, 0.0, -zOffset); // bottom left back
		glVertex3f(-centerWidth, 0.0, zOffset);	// bottom left front
		glVertex3f(-widthHalf, half_BH - headRadius, zOffset);	// top left
		// back
		glVertex3f(-widthHalf, half_BH - headRadius, -zOffset);	// top left
		glVertex3f(widthHalf, half_BH - headRadius, -zOffset);	// top right
		glVertex3f(centerWidth, 0.0, -zOffset);	// bottom right back
		glVertex3f(-centerWidth, 0.0, -zOffset); // bottom left back
		glVertex3f(-widthHalf, half_BH - headRadius, -zOffset);	// top left
	}

	glEnd();

	glPopMatrix();
}

void Character::leg(int leg)
{
	float zOffset = bodyThickness / 2;
	float leg_spacing = 1.0;	// -1.0 flips the leg spacing
	float side = 1;	// flips the leg side. will be 1 or -1
	//float foot_offset = 0.0;	// can go between 0 & half_BH/2
	float knee_spacing = abs(leg_spacing * 0.8);

	switch (leg)
	{
	case 0:
		break;
	case 1:
		leg_spacing = -leg_spacing;
		side = -side;
		break;
	default:
		return;
		break;
	}

	glBegin(GL_POLYGON);
	glVertex3f(side*centerWidth, 0.0, zOffset);	// outer hip front 
	glVertex3f(side*centerWidth, 0.0, -zOffset);	// outer hip back
	glVertex3f(side*centerWidth, 0.0, zOffset);	// outer hip front 
	glVertex3f(0.0, 0.0, zOffset);	// inner hip front
	glVertex3f(0.0, 0.0, -zOffset);	// inner hip back
	glVertex3f(0.0, 0.0, zOffset);	// inner hip front
	glVertex3f(side*knee_spacing, -half_BH / 2, zOffset);	// inner knee front
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, zOffset);	// outer knee front
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, -zOffset);	// outer knee back
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, zOffset);	// outer knee front
	glVertex3f(side*knee_spacing, -half_BH / 2, zOffset);	// inner knee front
	glVertex3f(side*knee_spacing, -half_BH / 2, -zOffset);	// inner knee back
	glVertex3f(side*knee_spacing, -half_BH / 2, zOffset);	// inner knee front
	glVertex3f(leg_spacing, -half_BH, zOffset);	// inner foot front
	glVertex3f(leg_spacing, -half_BH, -zOffset);	// inner foot back
	glVertex3f(leg_spacing, -half_BH, zOffset);	// inner foot front
	glVertex3f(side*centerWidth + leg_spacing, -half_BH, zOffset);	// outer foot front
	glVertex3f(side*centerWidth + leg_spacing, -half_BH, -zOffset);	// outer foot back
	glVertex3f(side*centerWidth + leg_spacing, -half_BH, zOffset);	// outer foot front
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, zOffset);	// outer knee front
	glVertex3f(side*centerWidth, 0.0, zOffset); // outer hip front
	// back
	glVertex3f(side*centerWidth, 0.0, -zOffset);	// outer hip back
	glVertex3f(0.0, 0.0, -zOffset);	// inner hip back
	glVertex3f(side*knee_spacing, -half_BH / 2, -zOffset);	// inner knee back
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, -zOffset);	// outer knee back
	glVertex3f(side*knee_spacing, -half_BH / 2, -zOffset);	// inner knee back
	glVertex3f(leg_spacing, -half_BH, -zOffset);	// inner foot back
	glVertex3f(side*centerWidth + leg_spacing, -half_BH, -zOffset);	// outer foot back
	glVertex3f(side*(centerWidth + knee_spacing), -half_BH / 2, -zOffset);	// outer knee back
	glVertex3f(side*centerWidth, 0.0, -zOffset);	// outer hip back
	glVertex3f(0.0, 0.0, -zOffset);	// inner hip back
	glVertex3f(side*centerWidth, 0.0, -zOffset);	// outer hip back

	glEnd();
}

void Character::arm(int arm)
{
	float widthHalf = torsoWidth / 2;
	float elbowHeight = 2 * (half_BH / 5);
	float arm_diameter = 0.5;	// thickness of arm from side to side
	float shoulder_diameter = 2.0;
	float s_inset = 0.4;
	float arm_spacing = 1.0;	// horizontal displacement of arm as a whole
	float elbow_spacing = 0.8 * arm_spacing;
	float wrist_height = -0.8;
	float wrSize = 0.5;
	float zOffset = bodyThickness / 2;

	switch (arm)
	{
	case 0:
		widthHalf = -widthHalf;
		arm_diameter = -arm_diameter;
		arm_spacing = -arm_spacing;
		elbow_spacing = -elbow_spacing;
		break;
	case 1:

		break;
	default:
		return;
		break;
	}

	glBegin(GL_POLYGON);

	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, zOffset);	// inner shoulder front
	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, -zOffset);	// inner shoulder back
	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, zOffset);	// inner shoulder front
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, zOffset);	// inner elbow front
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, -zOffset);	// inner elbow back
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, zOffset);	// inner elbow front
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, zOffset);	// outer elbow front
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, -zOffset);	// outer elbow back
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, zOffset);	// outer elbow front
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, zOffset);	// back to inner elbow front
	glVertex3f(widthHalf + arm_spacing, wrist_height, zOffset);	// inner wrist front
	glVertex3f(widthHalf + arm_spacing, wrist_height, -zOffset);	// inner wrist back
	glVertex3f(widthHalf + arm_spacing, wrist_height, zOffset);	// inner wrist front
	glVertex3f(widthHalf + arm_spacing + wrSize * arm_diameter, wrist_height, zOffset);	// outer wrist front
	glVertex3f(widthHalf + arm_spacing + wrSize * arm_diameter, wrist_height, -zOffset);	// outer wrist back
	glVertex3f(widthHalf + arm_spacing + wrSize * arm_diameter, wrist_height, zOffset);	// outer wrist front
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, zOffset);	// outer elbow front
	glVertex3f(widthHalf, half_BH - headRadius, zOffset);	// outer shoulder front
	glVertex3f(widthHalf, half_BH - headRadius, -zOffset);	// outer shoulder back
	glVertex3f(widthHalf, half_BH - headRadius, zOffset);	// outer shoulder front
	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, zOffset);	// inner shoulder front

	// back
	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, -zOffset);	// inner shoulder back
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, -zOffset);	// inner elbow back
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, -zOffset);	// outer elbow back
	glVertex3f(widthHalf + elbow_spacing, elbowHeight, -zOffset);	// back to inner elbow back
	glVertex3f(widthHalf + arm_spacing, wrist_height, -zOffset);	// inner wrist back
	glVertex3f(widthHalf + arm_spacing + wrSize * arm_diameter, wrist_height, -zOffset);	// outer wrist back
	glVertex3f(widthHalf + elbow_spacing + arm_diameter, elbowHeight, -zOffset);	// outer elbow back
	glVertex3f(widthHalf, half_BH - headRadius, -zOffset);	// outer shoulder back
	glVertex3f(widthHalf - s_inset*arm_diameter, half_BH - headRadius - shoulder_diameter, -zOffset);	// inner shoulder back

	glEnd();
}

void Character::hand(int hand)
{
	float widthHalf = torsoWidth / 2;
	float elbowHeight = 2 * (half_BH / 5);
	float arm_diameter = 0.5;	// thickness of arm from side to side
	float shoulder_diameter = 2.0;
	float s_inset = 0.4;
	float arm_spacing = 1.0;	// horizontal displacement of arm as a whole
	float elbow_spacing = 0.8 * arm_spacing;
	float wrist_height = -0.8;
	float wrSize = 0.5;
	float zOffset = bodyThickness / 2;

	GLUquadric *quadd = gluNewQuadric();
	glColor3f(r_accent, g_accent, b_accent);	// use accent color

	switch (hand)
	{
	case 0:
		glPushMatrix();
		glTranslatef(widthHalf + arm_spacing + (wrSize / 2)*arm_diameter, wrist_height, 0.0);
		gluSphere(quadd, arm_diameter / 1.5, 20, 20);
		glPopMatrix();
		break;
	case 1:
		glPushMatrix();
		glTranslatef(-widthHalf - arm_spacing - (wrSize / 2)*arm_diameter, wrist_height, 0.0);
		gluSphere(quadd, arm_diameter / 1.5, 20, 20);
		glPopMatrix();
		break;
	default:
		return;
		break;
	}
}

void Character::foot(int foot)
{
	float zOffset = bodyThickness / 2;
	float leg_spacing = 1.0;	// -1.0 flips the leg spacing
	float knee_spacing = abs(leg_spacing * 0.8);
	GLUquadric *quadd = gluNewQuadric();

	float footSize = 0.5;

	glPushMatrix();
	glColor3f(r_accent, g_accent, b_accent);	// use accent color

	switch (foot)
	{
	case 0:
		glTranslatef(leg_spacing + centerWidth, -half_BH, 0.0);
		gluSphere(quadd, footSize, 20, 20);
		break;
	case 1:
		glTranslatef(-leg_spacing - centerWidth, -half_BH, 0.0);
		gluSphere(quadd, footSize, 20, 20);
		break;
	default:
		glPopMatrix();
		return;
		break;
	}

	glPopMatrix();

}



#endif