#ifndef __BITMAP_H_INCLUDE__
#define __BITMAP_H_INCLUDE__

#include <string>
#include <deque>
#include <iostream>
//#include <cstdlib>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class BitMap{
public:
	int sizeX;
	int sizeY;
	unsigned char *data;
	// Texture binding variables
	unsigned int *texture; // Array of texture indices.

public:
	std::deque<std::string> sources;	// filenames
	int selectedSource;

	BitMap();
	BitMap(std::string filename);

	void reload();
	BitMap* loadBMP(std::string filename);	// Load 24-bit uncompressed bitmap file
	// Texture functions
	void setupTexture();

private:
	void loadProceduralTextures();	// Load image data as a texture. Called by setupTexture()
};



BitMap::BitMap()
{
	sizeX = sizeY = -1;
	data = nullptr;
	texture = new unsigned int[1];
	selectedSource = 0;

	// Get the file names
	std::ifstream in("gliderTextureFilenames.txt");
	while (!in.eof())
	{
		std::string st;
		in >> st;
		sources.push_back(st);
	}

	if (sources.size() > 0)
		loadBMP(sources[selectedSource]);
	else
	{
		std::cout << "No files given" << std::endl;
		return;
	}

	if (sizeX == 0 && sizeY == 0)
		std::cout << "Image file: " << sources[selectedSource] << " wasn't found" << std::endl;

}

BitMap::BitMap(std::string filename)
{
	texture = new unsigned int[1];
	loadBMP(filename);

	// Note if the image wasn't loaded
	if (sizeX == 0 && sizeY == 0)
		std::cout << "Image file: " << filename << " wasn't found" << std::endl;
}

// Load the currently selected file
void BitMap::reload()
{
	if (selectedSource < sources.size() && selectedSource >= 0)
		loadBMP(sources[selectedSource]);
}

// Load the BitMap data
// Returns this BitMap object
BitMap* BitMap::loadBMP(std::string filename)
{
	unsigned int size, offset, headerSize;

	// Read input file name.
	std::ifstream infile(filename.c_str(), std::ios::binary);

	// Get the starting point of the image data.
	infile.seekg(10);
	infile.read((char *)&offset, 4);

	// Get the header size of the bitmap.
	infile.read((char *)&headerSize, 4);

	// Get width and height values in the bitmap header.
	infile.seekg(18);
	infile.read((char *)&this->sizeX, 4);
	infile.read((char *)&this->sizeY, 4);

	// Allocate buffer for the image.
	size = this->sizeX * this->sizeY * 24;
	this->data = new unsigned char[size];

	// Read bitmap data.
	infile.seekg(offset);
	infile.read((char *)this->data, size);

	// Reverse color from bgr to rgb.
	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = this->data[i];
		this->data[i] = this->data[i + 2];
		this->data[i + 2] = temp;
	}

	return this;
}

void BitMap::setupTexture()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glGenTextures(1, texture);	// Create texture index array.

	loadProceduralTextures();	// Load image data as a texture.

	//glEnable(GL_TEXTURE_2D);	// Enable OpenGL texturing

	//// Specify how texture values combine with current surface color values.
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// Load image data as a texture. 
void BitMap::loadProceduralTextures()
{
	// Bind chessboard image to texture index[0].
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, this->data);
}


#endif