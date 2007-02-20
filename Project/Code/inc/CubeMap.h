#pragma once

typedef unsigned int GLuint;
typedef int GLsizei;

class CubeMap
{
public:
	CubeMap(GLsizei aImageSize = 256);
	~CubeMap();

	void setFaces(float* aPositiveX, float* aNegativeX, float* aPositiveY, 
		float* aNegativeY, float* aPositiveZ, float* aNegativeZ);

	void render();

	// methods
private:

	void setupCubeMap();

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