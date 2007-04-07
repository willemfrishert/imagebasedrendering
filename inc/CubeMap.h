#pragma once

#include <string>

typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;

using namespace std;

class CubeMap
{
public:
	CubeMap(GLint nChannels, GLsizei aImageSize = 256);
	~CubeMap();

	void setupCubeMap(string filenamePrefix);

	void render(float* inverseTransMatrix);

	// methods
private:

	void extractFaces(float** columns, int height, int width);

	// attributes
private:
	/*
	 * Cubemap textures
	 */
	float* positiveX;
	float* negativeX;
	
	float* positiveY;
	float* negativeY;
	
	float* positiveZ;
	float* negativeZ;

	/**
	 * @description 
	 */
	GLsizei imageSize;

	/**
	 * Default value
	 * Variable comment
	 */
	GLint channels;

	GLint format;
};