#pragma once

typedef unsigned int GLuint;
typedef int GLsizei;

class CubeMap
{
public:
	CubeMap(GLsizei aImageSize = 256);
	~CubeMap();

	void extractFaces(float* columns, int height, int width);

	void setupCubeMap();

	void render();

	// methods
private:

	// attributes
private:
	float* positiveX;
	float* negativeX;
	
	float* positiveY;
	float* negativeY;
	
	float* positiveZ;
	float* negativeZ;

	GLsizei imageSize;

	GLuint texName;

	//Mesh* cube;
};