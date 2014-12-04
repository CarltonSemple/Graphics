#ifndef __BUTTON3D_H_INCLUDE__
#define __BUTTON3D_H_INCLUDE__

class Button3D
{
	float xCenter, yCenter, zCenter;
	float xRotation, yRotation, zRotation;
	char direction;	// d: down. u: up
	bool clicked;
public:
	Button3D();
	Button3D(float x, float y, float z, float xR, float yR, float zR);
	~Button3D();

	void draw(bool selectionModeEnabled, unsigned int closestName, int highlightFrames);
	bool isClicked(){ return clicked; }

	void rotate(float increment);
	void move(float xChange, float yChange, float zChange);
	void verticalScroll(float yLow, float yHigh, float increment);
private:

};



#endif