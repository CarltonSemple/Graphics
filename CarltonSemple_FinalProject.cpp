#include <iostream>
#include <ctime>
#include <deque>
#include <thread>
#include <string>
#include <string.h>

#include "Bird.h"
#include "Button2D.h"
#include "Button3D.h"
#include "BitMap.h"
#include "Character.h"
#include "Cloud.h"
#include "Droplet.h"
#include "MobileObject.h"
#include "WaveSet.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#include <GL/glext.h>
#endif

using namespace std;

class point{
public:
	float x, y, z;
	point();
	point(float a, float b, float c){ x = a; y = b; z = c; }
};

// Window Properties
static int wID1, wID2; // Window identifiers.
static int mainWidth = 500, mainHeight = 500, secondaryWidth = 600, secondaryHeight = 500;	// Allow for manipulation of window properties

static float xWorld = 0.0, yWorld = 0.0, zWorld = 0.0;

// Mouse
point *mouseClick = new point(0.0, 0.0, 0.0);

/*********************************************** Menus ******************************************/

// Glider Menu
static float buttonHeight = mainHeight / 7;
Button2D *previous_button = new Button2D(0.0, mainWidth / 2, 0, buttonHeight, "previous", 0.0, 'd');
Button2D *next_button = new Button2D(mainWidth / 2, mainWidth, 0, buttonHeight, "next", 0.0, 'i');
Button2D *continue_button = new Button2D(2.7 * (mainWidth / 4), mainWidth, buttonHeight, mainHeight, "Continue", 0.0, 'c');

// Character Menu
Button3D *start_button = new Button3D(-6.0, 15.0, 5.0, 
										40.0, 64.0, 30.0);
Button2D *blueButton = new Button2D(mainWidth / 2, mainWidth / 2 + mainWidth / 10, 0.0, buttonHeight / 2, "b", 0.0, 'b');
Button2D *redButton = new Button2D(mainWidth / 2, mainWidth / 2 + 2*(mainWidth / 10), 0.0, buttonHeight / 2, "r", 0.0, 'r');
Button2D *greenButton = new Button2D(mainWidth / 2, mainWidth / 2 + 3 * (mainWidth / 10), 0.0, buttonHeight / 2, "g", 0.0, 'g');

	// Clicking
#define BUFFER_SIZE 1024
static unsigned int hitBuffer[1024];	// hit buffer
static unsigned int closestName = 0;	// name of closest hit
static int hitCount;	// number of hits found in hit buffer
static int highlightFrames = 10;
static bool selectionMode = false;	// in selection mode or not
static bool drawInRenderMode = true;

// Camera
static float xCamera = 0.0, yCamera = 0.0, zCamera = 40.0; // The camera won't really move in general, but just a little for extra effects
static float focalDistance = 120.0;
static float drawDistance = 20.0;
static float xFocus = 0.0, yFocus = 40.0, zFocus = -40.0;
static float camAngle = 90.0;	// horizontal, x & z
static float vertAngle = 0.0;

// Character
static float xPlayer = 0.0, yPlayer = 30.0, zPlayer = 0.0;
static Character *character = new Character(xPlayer, yPlayer, zPlayer, 
											1.0, 3.0, 10.0, 
											1.3,
											10,150.0);
bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;


static int score = 0;

// Birds
static deque<Bird*> theBirds;	// the flock of birds . inspired by Alfred Hitchcock

// Clouds
static deque<Cloud*> clouds;
static float cloudSpacing = 30.0;

// Waves
int waveHeightRange = 25;
float oceanVelocity = 0.2;
static float sectionLength = 10.0;
static float oceanLength;
float oceanTraversedDistance = 0;
static deque<WaveSet*> waves;

// **************************** Animation Logic ****************************
static int frameDuration = 40;
static bool gliderMenu = true;
static bool characterMenu = false;
static bool characterSelected = false;

static bool sendBirds = false;


static float Xangle = 30.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate canoe.
static int rowCount = 0, columnCount = 0; // Indexes of selected control point.

// Forward Function Declarations
void resize1(int w, int h);
void resize2(int w, int h);

// ********************************** Functions **********************************
// **																			**
// *																			 *

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// ******************************** Camera Tricks ********************************

// Rotate the focus points for gluLookAt
void turn(float angleChange)
{
	camAngle += angleChange;
	if (camAngle >= 360)
		camAngle = camAngle - 360.0;
	else if (camAngle < 0)
		camAngle = camAngle + 360.0;
	float angle = camAngle;		// camera angle

	float s, c;	// sine & cosine

	if (angle == 0) // east
	{
		// move point of focus
		xFocus = focalDistance + xCamera;
		zFocus = 0.0 + zCamera;
	}
	else if (angle == 90) // north
	{
		// move point of focus
		zFocus = -focalDistance + zCamera;
		xFocus = 0.0 + xCamera;
	}
	else if (angle == 180) // west
	{
		// move point of focus
		xFocus = -focalDistance + xCamera;
		zFocus = 0.0 + zCamera;
	}
	else if (angle == 270) // south
	{
		// move point of focus
		zFocus = focalDistance + zCamera;
		xFocus = 0.0 + xCamera;
	}
	// northern hemisphere
	else if (angle > 0 && angle < 180)
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		zFocus = (-focalDistance * s) + zCamera;
		xFocus = (focalDistance * c) + xCamera;
	}
	// southern hemisphere
	else if (angle > 180 && angle < 360)
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		zFocus = (-focalDistance * s) + zCamera;
		xFocus = (focalDistance * c) + xCamera;
	}
}

// Rotate the yFocus point for gluLookAt
void rotateVertical(float angleChange)
{
	vertAngle += angleChange;
	if (vertAngle >= 90.0)
		vertAngle = 90.0;
	else if (vertAngle < -90)
		vertAngle = -90.0;
	float angle = vertAngle;		// camera angle

	float s, c;	// sine & cosine

	if (angle == -90) // straight
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		yFocus = (focalDistance * s) + yCamera;
	}
	else if (angle == 0) // straight up
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		yFocus = (focalDistance * s) + yCamera;
	}
	else if (angle == 90) // west
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		yFocus = (focalDistance * s) + yCamera;
	}
	// down
	else if (angle > -90 && angle < 0)
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		yFocus = (focalDistance * s) + yCamera;
	}
	// up
	else if (angle > 0 && angle < 90)
	{
		// convert degrees to radians
		angle *= 0.0174532925;
		s = sin(angle);	c = cos(angle);

		// move point of focus
		yFocus = (focalDistance * s) + yCamera;
	}
}


// ***************************** Bird System ****************************

/*
Mange a set of birds, spaced randomly. Only draw birds within reasonable distance
*/
void manageBirds(
	point startPosition,
	int waveSize, // how many birds spawn next to each other
	int numWaves, // how many sets of birds
	float delayBetweenBirds)
{
	srand(time(0));
	float birdSize = 1.0;
	static int birdCount = waveSize * numWaves;
	static int waveCount = 0; // 
	static float spacing = 7 * birdSize;
	static float x = startPosition.x, y = startPosition.y, z = startPosition.z, direction = 0;	// change these between function calls
	float xCurrent = x, yCurrent = y, zCurrent = z;
	if (sendBirds == false)
		return;

	// Generate the set of birds
	if (waveCount < numWaves)	// do this many waves
	{
		// Allow for previous wave to have gone
		int goneCount = 0;
		for (int h = 0; h < theBirds.size(); h++)
			if (theBirds[h]->gone)
			{
			theBirds[h]->draw();	// draw the bird if it's gone
			goneCount++;
			}

		if (goneCount == theBirds.size())	// all birds gone	
		{
			for (int i = 0; i < waveSize; i++)		// Release a new wave
			{
				Bird *nB = new Bird(xCurrent, yCurrent, zCurrent, direction, birdSize);
				nB->goneDistance = delayBetweenBirds;
				theBirds.push_back(nB);
				int del = 100;
				while (del > 0)
					del--;

				// change horizontally
				xCurrent += spacing;
				
			}
			waveCount++;
		}
		// Draw all of the birds
		for (int i = 0; i < theBirds.size(); i++)
		{
			theBirds[i]->draw();
		}
		return;
	}

	// Remove the birds that have reached the point of no return
	while (true)
	{
		if (theBirds.size() > 0)
		{
			deque<Bird*>::iterator dIt = theBirds.begin();	// check the front, since this contains the oldest bird

			Bird* bp = *dIt;
			if (bp->destroy)
			{
				theBirds.pop_front();
				delete(bp);

				// Add a new bird
				//
				//Bird* newBi = new Bird(x, y, z, direction, 0.5);
				//
				//theBirds.push_back(newBi); // add to the end
				//
			}
			else
				break;	// break if there is no bird at the front that's reached the point of expulsion
		}
		else
		{
			theBirds.clear();
			break;
		}
	}

	// Draw all of the birds
	for (int i = 0; i < theBirds.size(); i++)
	{
		theBirds[i]->draw();
	}
}


// **************************** Particle Cloud System ****************************

/*
Add new clouds. Remove old ones. Keep the cloud count consistent & continuous
*/
void manageClouds(	point startPosition,
					int numClouds,
					float &cloudSeparation, float minSeparation, float range)
{
	float changeX = rand() % 8;
	float changeY = rand() % 7;
	static float startx = startPosition.x + changeX - 3, starty = startPosition.y + changeY - 2, startz = startPosition.z;
	
	if (startx >= startPosition.x + range)
		startx = startPosition.x - range / 2;
	if (starty >= startPosition.y + range)
		starty = startPosition.y - range / 2;

	if (clouds.size() < numClouds)
	{
		// Release a new cloud once the previously released cloud has gone
		if (clouds.size() != 0)
		{
			deque<Cloud*>::reverse_iterator crI = clouds.rbegin();	// check last cloud 
			Cloud* cl = *crI;
			if (cl->gone)
			{
				Cloud *nC = new Cloud(1.0, 1.0, 1.0,
					startx, starty, startz,
					5, 100, 0.4);
				nC->goneDistance = cloudSeparation;	// decrease this with time
				if (cloudSeparation - 0.5 > minSeparation)
					cloudSeparation -= 0.5;	// clouds arrive more quickly
				else
					cloudSeparation = minSeparation;	// minimum separation

				clouds.push_back(nC);

				// increment position
				startx += 2.0;
				starty += 2.5;
			}
		}
		else
		{
			Cloud *nC = new Cloud(1.0, 1.0, 1.0,
				startx, starty, startz,
				5, 100, 0.4);
			nC->goneDistance = cloudSeparation;	// decrease this with time

			clouds.push_back(nC);
		}

	}

	deque<Cloud*>::iterator cI = clouds.begin();
	while (cI != clouds.end())
	{
		Cloud* clou = *cI;
		if (!clou->destroy)
			clou->draw();
		cI++;
	}

	// Remove the clouds that reached their goal
	while (true)
	{
		cI = clouds.begin();
		if (cI == clouds.end())
			break;
		Cloud * cc = *cI;
		if (cc->destroy)
		{
			clouds.pop_front();
			delete cc;
		}
		else
			break;
	}

}


// **************************** Wave Set System ****************************

// Build a set of waves along the xz plane
deque<WaveSet*> build_XZ_waves(float xSta, float zSta,  // starting coordinates
								int xCount, int zCount,		// counts for # of sections
								float sectionWidth, float sectionLength,
								int append) 
{
	deque<WaveSet*> wTemp;

	float y = 0;
	float width = sectionWidth;
	float length = sectionLength;
	float xStart = xSta, zStart = zSta;	// keep the previous position

	int zc = 0;
	for (float l = zStart; zc < zCount; l -= (length / 2))	// working backwards (- values)
	{
		int xc = 0;
		for (float x = xStart; xc < xCount; x += (width / 2))
		{
			WaveSet *nWave = new WaveSet(0, y, 0, x, width, l, length, waveHeightRange);
			if (append == 0)
				wTemp.push_back(nWave);
			else if (append == 1)
				waves.push_back(nWave);
			xc++;
		}
		zc++;
	}


	return wTemp;
}

// Move waves up & down
void waveCycleMotion(float speed)
{
	for (int w = 0; w < waves.size(); w++)
	{
		if (waves[w]->waveHeight > -waves[w]->originalHeight && waves[w]->goDown == true)
		{
			waves[w]->waveHeight -= waves[w]->speed;
			if (waves[w]->waveHeight <= -waves[w]->originalHeight)
				waves[w]->goDown = false;
		}
		else if (waves[w]->waveHeight < waves[w]->originalHeight)
		{
			waves[w]->waveHeight += waves[w]->speed;
			if (waves[w]->waveHeight >= waves[w]->originalHeight)
				waves[w]->goDown = true;
		}
	}
}

// Remove waves that have reached their goal
void removeSuccessfulWaves()
{
	while (true)
	{
		if (waves.size() == 0)
			break;

		WaveSet *wCurrent = waves.front();

		if (wCurrent->destroy == true)	// wave will mark itself for destruction
		{
			waves.pop_front();
			delete(wCurrent);
		}
		else
			break;
	}
}

void addNewWaves()
{

}

void manageWaves()
{	
	// Create the set of waves
	if (waves.size() == 0)
	{
		oceanLength = 10 * 5.0;	// number of sections z-wise * section length/2
		waves = build_XZ_waves(	-25.0, 20.0,
								16, 10,
								6.0, sectionLength,
								0);
	}
	

	float matAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float matDif[] = { 1.0, 1.0, 0.0, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShine[] = { 90.0 };
	float matEmission[] = { 0.0, 0.0, 0.0, 1.0 };

	// Material properties of sphere.
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	//glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	//glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	removeSuccessfulWaves();	// Remove waves that have reached their goal
	
	//if (oceanTraversedDistance >= sectionLength/2)
	if (waves.size() <= 160 - 16)
	{
		//oceanTraversedDistance = 0;
		build_XZ_waves(	-25, 20.0 - (10*5),
						16, 1,
						6.0, sectionLength,
						1);
	}
	//addNewWaves();

	for (int i = 0; i < waves.size(); i++)
	{
		waves[i]->draw();
	}

}


// Allow for buttons to alternate between green & blue
float Button2D::blue = 1.0;
float Button2D::green = 0.0;
// Draw the 2D Menu for Glider Customization
void drawGliderMenu()
{
	character->drawGlider();


	previous_button->draw(true, 0.0, 0.0, 0.0);	next_button->draw(true, 0.0, 0.0, 0.0);
	continue_button->draw(false, 0.2, 0.2, 0.2);
}

// Menu to customize the character's color
void viewCharacterMenu()
{
	gluLookAt(
		//0.0, 2.0, 10.0,	// Camera position
		5.0, 15.0, 21.0,
		0.0, 15.0, 0.0,	// Camera focus position
		0.0, 1.0, 0.0);	// Which side is "up"

	// without selection
	if (drawInRenderMode)
		selectionMode = false;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// enable lighting
	glEnable(GL_LIGHTING);
	character->drawForSelection(5.0, 15.0, -6.0, 0.0, 0.0, 0.0);	// Draw the character without the glider


	// Introduce fog *******************************************
	float fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
	int fogMode = GL_EXP2; // Fog mode.
	float fogDensity = 0.035; // Fog density.

	

	// Draw the color buttons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPushMatrix();
	//	glTranslatef(0.0, 6.0, 1.2);
	//	glRotatef(13.0, 0.0, 1.0, 0.0);
	//	//glColor3f()
	//	glutSolidCube(3.0);
	//glPopMatrix();
	//glPushMatrix();
	//	glTranslatef(3.5, 6.0, 0.3);
	//	glRotatef(13.0, 0.0, 1.0, 0.0);
	//	glutSolidCube(3.0);
	//glPopMatrix();
	//glPushMatrix();
	//	glTranslatef(7.0, 6.0, -0.5);
	//	glRotatef(13.0, 0.0, 1.0, 0.0);
	//	glutSolidCube(3.0);
	//glPopMatrix();
	/*redButton->draw(false, 1.0, 0.0, 0.0);
	greenButton->draw(false, 0.0, 1.0, 0.0);
	blueButton->draw(false, 0.0, 0.0, 1.0);*/

	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, fogMode);
	glFogf(GL_FOG_START, -5.0);	// starting z value
	glFogf(GL_FOG_END, 10.0);	// ending z value
	glFogf(GL_FOG_DENSITY, fogDensity);

	// Draw the start button within the fog
	start_button->draw(selectionMode, closestName, highlightFrames);	// Draw the start button

	
	glLoadIdentity();
	glDisable(GL_FOG);

	
}

// Drawing Routine for main (left) window
void drawScene(void)
{
	if (gliderMenu == true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		drawGliderMenu();
	}
	else if (gliderMenu == false && characterMenu == true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glLoadIdentity();
		
		viewCharacterMenu();
	}
	else
	{
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		resize1(mainWidth, mainHeight);	// Ensure window properties. 
		// Should technically be called elsewhere


		//gluLookAt(0.0, 0.0, 12.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		gluLookAt(
			xCamera, yCamera, zCamera,
			xFocus, yFocus, zFocus,
			0.0, 1.0, 0.0);

		// Rotate scene.
		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		characterSelected = true;

		

		// Proceed with scene

		glEnable(GL_LIGHTING);

		// Draw the character
		character->draw();


		// Draw score
		glPushMatrix();
		glRasterPos3f(-35.0, 49.0, -30);
		string s = "Score: " + std::to_string(score);
		char *cstr = new char[s.length() + 1];	// http://stackoverflow.com/questions/7352099/stdstring-to-char
		for (int i = 0; i < s.length(); i++)
			cstr[i] = s[i];
		glColor3f(1.0, 0.0, 1.0);
		writeBitmapString(GLUT_BITMAP_HELVETICA_18, cstr);
		glPopMatrix();
		
		// Create and maintain waves
		manageWaves();

		// Send the birds
		sendBirds = true;

		manageBirds(
			point(-50, 45, -80),
			6, 800, 8);

		manageClouds(	point(0, 17, -70),
						300, cloudSpacing, 10, 8);

		// Draw the Water Droplets
		character->drawDroplets();

		glDisable(GL_LIGHTING);
		
	}	

	glutSwapBuffers();
}


// Main Window setup
void setup1(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST); // Enable depth testing.

	character->bmp->setupTexture();
	
	// Enable color material mode:
	// The ambient and diffuse color of the front faces will track the color set by glColor().
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


	// Turn on OpenGL lighting.
	//glEnable(GL_LIGHTING);

	//*************************************************** The most recent attempt at lighting is based off the litBezierCanoe.cpp example

	// Light
	float ambie[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambie);
	float posit[] = { 0.0, 15.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, posit);

	// Light property vectors.
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos[] = { 0.0, 1.5, 3.0, 0.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };

	// Light properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambie);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

	// Material property vectors.
	float matAmbAndDif1[] = { 0.9, 0.0, 0.7, 1.0 };
	float matAmbAndDif2[] = { 0.3, 0.3, 0.3, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShine[] = { 50.0 };

	// Material properties.
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif2);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
}


// Main Window Resize
void resize1(int w, int h)
{
	mainWidth = w;	mainHeight = h;		// keep the globals updated

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (gliderMenu == true)
		glOrtho(0.0, (float)w, 0.0, (float)h, 10.0, -100.0);
	else
		gluPerspective(60.0, (float)w / (float)h, 1.0, 250.0);
	glMatrixMode(GL_MODELVIEW);

	// Update the glider's position to be drawn alone
	character->updateGliderPosition_Exclusive(
		0.59*mainWidth, 0.5*mainHeight, -4.0,
		90.0, 0.0, -0.0);

	// Update the button sizes & position
	previous_button->updatePosition(0.0, mainWidth / 2, 0, buttonHeight, 0.0);
	next_button->updatePosition(mainWidth / 2, mainWidth, 0, buttonHeight, 0.0);
	continue_button->updatePosition(2.7 * (mainWidth / 4), mainWidth, buttonHeight, mainHeight, 0.0);

}

// Move the player according to which key was pressed
void handlePlayerMovement(int button, float moveAmount)
{
	switch (button)
	{
	case 0:	// up
		moveUp = true;
		moveDown = false;
		moveLeft = false;
		moveRight = false;
		yPlayer += moveAmount;
		character->updatePosition(xPlayer, yPlayer, zPlayer);
		break;
	case 1:	// down
		moveUp = false;
		moveDown = true;
		moveLeft = false;
		moveRight = false;
		yPlayer -= moveAmount;
		character->updatePosition(xPlayer, yPlayer, zPlayer);
		break;
	case 2:	// left
		moveUp = false;
		moveDown = false;
		moveLeft = true;
		moveRight = false;
		xPlayer -= moveAmount;
		character->updatePosition(xPlayer, yPlayer, zPlayer);
		break;
	case 3:	// right
		moveUp = false;
		moveDown = false;
		moveLeft = false;
		moveRight = true;
		xPlayer += moveAmount;
		character->updatePosition(xPlayer, yPlayer, zPlayer);
		break;
	default:
		return;
		break;
	}

	glutPostRedisplay();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'x':
		Xangle += 5.0;
		if (Xangle > 360.0) Xangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'X':
		Xangle -= 5.0;
		if (Xangle < 0.0) Xangle += 360.0;
		glutPostRedisplay();
		break;
	case 'y':
		Yangle += 5.0;
		if (Yangle > 360.0) Yangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Y':
		Yangle -= 5.0;
		if (Yangle < 0.0) Yangle += 360.0;
		glutPostRedisplay();
		break;
	case 'z':
		Zangle += 5.0;
		if (Zangle > 360.0) Zangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Z':
		Zangle -= 5.0;
		if (Zangle < 0.0) Zangle += 360.0;
		glutPostRedisplay();
		break;
	case 9:
	{
		if (rowCount < 5) rowCount++;
		else rowCount = 0;
	}
		glutPostRedisplay();
		break;
	case ' ':
	{
		if (columnCount < 3) columnCount++;
		else columnCount = 0;
	}
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	float moveAmount = 2.0;
	if (key == GLUT_KEY_UP)
		handlePlayerMovement(0, moveAmount);
	if (key == GLUT_KEY_DOWN)
		handlePlayerMovement(1, moveAmount);
	if (key == GLUT_KEY_LEFT)
		handlePlayerMovement(2, moveAmount);
	if (key == GLUT_KEY_RIGHT)
		handlePlayerMovement(3, moveAmount);

	//cout << "gZ: " << zTemp << endl;

	glutPostRedisplay();
}

void findClosestHit(int hits, unsigned int buffer[]);

void handleCharacterMenuClicks(int button, int state, int x, int y)
{
	/******************************** Character Menu Buttons ******************************/

	// wait for left button click
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;

	//y = mainHeight - y; // Correct from mouse to OpenGL co-ordinates.

	// check to see if the click was inside any of the menu options
	mouseClick->x = x;
	mouseClick->y = mainHeight - y;	// Save the values for use outside this function


	/****************** Clicking on the Moving Button *******************/

	int viewport[4]; // data for viewport

	glGetIntegerv(GL_VIEWPORT, viewport);	// get viewport data

	glSelectBuffer(BUFFER_SIZE, hitBuffer);
	(void)glRenderMode(GL_SELECT);	// selection mode

	// Save the viewing volume defined in the resize routine.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Define a viewing volume corresponding to selecting in 3 x 3 region around the cursor.
	glLoadIdentity();
	gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);
	gluPerspective(60.0, (float)mainWidth / (float)mainHeight, 1.0, 250.0); // Same as found in the resize function.

	glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
	glLoadIdentity();

	glInitNames(); // Initializes the name stack to empty.
	glPushName(0); // Puts name 0 on top of stack.

	// Determine hits by calling button draw function, assigning name(s) (see draw function)
	selectionMode = true;
	drawInRenderMode = false;
	drawScene();


	hitCount = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.

	drawInRenderMode = true;

	// Restore viewing volume of the resize routine
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	// return to modelview mode

	// Determine closest of the hit objects (if any).
	findClosestHit(hitCount, hitBuffer);

}

// The mouse callback routine. For Main Window 
void mouseControl(int button, int state, int x, int y)
{
	/******************************** Glider Menu Buttons ******************************/
	if (gliderMenu == true)
	{
		// left button click
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			y = mainHeight - y; // Correct from mouse to OpenGL co-ordinates.

			// check to see if the click was inside any of the menu options
			mouseClick->x = x;
			mouseClick->y = y;	// Save the values for use outside this function

			if (previous_button->containsCoordinate(mouseClick->x, mouseClick->y))
			{
				// Change to the previous texture
				previous_button->incrementOrDecrement(character->bmp->selectedSource);
				if (character->bmp->selectedSource < 0)	// loop the index 
					character->bmp->selectedSource = character->bmp->sources.size() - 1;	
				character->bmp->reload();
				character->bmp->setupTexture();
			}
			else if (next_button->containsCoordinate(mouseClick->x, mouseClick->y))
			{
				// Change to the next texture
				next_button->incrementOrDecrement(character->bmp->selectedSource);
				if (character->bmp->selectedSource >= character->bmp->sources.size())	// loop the index back to 0
					character->bmp->selectedSource = 0;
				character->bmp->reload();
				character->bmp->setupTexture();
			}
			else if (continue_button->containsCoordinate(mouseClick->x, mouseClick->y))
			{
				// Progress to the Next Menu
				gliderMenu = false;
				characterMenu = true;
				// Change the window settings
				resize1(mainWidth, mainHeight);
			}
			

		}
	}
	else if (characterMenu == true)
	{
		handleCharacterMenuClicks(button, state, x, y);
	}
	else if (gliderMenu == false && characterMenu == false)
	{
		// wait for left button click
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
			return;
		character->spewWater();	// release a water droplet
	}

	glutPostRedisplay();
}

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
	unsigned int *ptr, minZ;

	minZ = 0xffffffff; // 2^32 - 1
	ptr = buffer;
	closestName = 0;
	for (int i = 0; i < hits; i++)
	{
		ptr++;
		if (*ptr < minZ)
		{
			minZ = *ptr;
			ptr += 2;
			closestName = *ptr;
			ptr++;
		}
		else ptr += 3;
	}
	if (closestName != 0)
	{
		highlightFrames = 10;
		//cout << "clicked something" << endl;
	}
}

void printInteraction(void)
{
	
}

/*************** Movement Functions ****************/
void moveWaves()
{
	for (int i = 0; i < waves.size(); i++)
	{
		waves[i]->moveToward2D(waves[i]->getXcenter(), waves[i]->getY(), zCamera - 10, oceanVelocity);
	}

}

void updateBirds()
{
	for (int i = 0; i < theBirds.size(); i++)
	{
		theBirds[i]->moveToward2D(theBirds[i]->getX() + 100, 0, 30, 0.1);
		theBirds[i]->updateWings(0.2);
	}
}

void moveClouds()
{
	deque<Cloud*>::iterator cI = clouds.begin();

	while (cI != clouds.end())
	{
		Cloud* clou = *cI;
		clou->moveToward2D(clou->getX() + clou->xGoal, clou->getY() + clou->yGoal, zCamera + 20, 0.5);

		cI++;
	}

}

// Check to see if any of the droplets hit each cloud
void ofCloudsAndWater()
{
	if (clouds.size() == 0)
		return;
	deque<Cloud*>::iterator cIt = clouds.begin();
	while (cIt != clouds.end())
	{
		if (character->goneDroplets.size() > 0)
		{
			Cloud* cloud = *cIt;
			// check all of the flying droplets
			deque<Droplet*>::iterator dIt = character->goneDroplets.begin();
			while (dIt != character->goneDroplets.end())
			{
				Droplet *drop = *dIt;
				cloud->reactToDroplet(drop, *character, score);	// mark if the droplet hit the cloud and increase the score if so
				dIt++;
			}
		}
		cIt++;
	}
}

void timer(int value)
{
	/**************** Character Menu ***************/
	start_button->rotate(1.0);
	start_button->verticalScroll(7.0, 21.0, 0.1);
	if (start_button->isClicked())
		characterMenu = false;	// Move on to the next stage

	/*************** Main Experience ***************/
	thread bThread(updateBirds);	// move birds & their wings
	thread wThread(waveCycleMotion, 1.7);	// Move waves up & down
	thread mThread(moveWaves);
	thread cThread(moveClouds);

	bThread.join();
	wThread.join();
	mThread.join();
	cThread.join();

	character->moveDroplets(2.0);

	thread oThread(ofCloudsAndWater);
	oThread.join();

	glutTimerFunc(frameDuration, timer, 1);
	glutPostRedisplay();
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Main window
	glutInitWindowSize(mainWidth, mainHeight);
	glutInitWindowPosition(100, 100);
	wID1 = glutCreateWindow("Hang Glide");
		setup1();
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize1);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialKeyInput);
		glutMouseFunc(mouseControl);


	// timer/animation
	glutTimerFunc(5, timer, 1);

	glutMainLoop();

	return 0;
}
