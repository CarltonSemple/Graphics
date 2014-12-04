#ifndef __BUTTON2D_H_INCLUDE__
#define __BUTTON2D_H_INCLUDE__

//#include<cstdlib>

class Button2D
{
	float xCenter, yCenter, zCenter;
	float hWidth, hHeight;
	float left, right, top, bottom;
	char* text;
	char action;	// 'i' = increment. 'd' = decrement
	
public:
	static float green, blue;
	//float green, blue;

	Button2D();
	Button2D(float x, float y, float z, float width, float height, char *n);	// Create a Button from the given coordinates
	Button2D(float l, float r, float b, float t, char *n, float z, char action);	// Create a Button From the Given Boundaries
	~Button2D();


	void draw(bool blueGreen, float r, float g, float b);
	void updatePosition(float l, float r, float b, float t, float z);
	bool containsCoordinate(float x, float y);
	void incrementOrDecrement(int &affectedNumber);

private:
	void writeBitmapString(void *font, char *string);
};



#endif