#pragma once

#include <string>

typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;

using namespace std;

class CubeMap
{
public:
	CubeMap(GLint nChannels);
	~CubeMap();

	void setupCubeMap(string filenamePrefix);
	void setupCompressedCubeMap(string filenamePrefix, unsigned char& minExponent, unsigned char& maxExponent);

	void render(float* inverseTransMatrix);

	// methods
private:

	void extractFaces(float** columns, int height, int width);

	void extractFaces(unsigned char** columns, int height, int width);

	// attributes
private:
	/*
	 * Cubemap textures
	 */
	void* positiveX;
	void* negativeX;
	
	void* positiveY;
	void* negativeY;
	
	void* positiveZ;
	void* negativeZ;

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