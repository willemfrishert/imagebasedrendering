#include <gl/glew.h>
#include <gl/glut.h>
#include <stdio.h>
#include "CubeMap.h"


static Vector3 vertices[8] = {

	Vector3(-1.0,  1.0, -1.0), // l t f
	Vector3(-1.0, -1.0, -1.0), // l b f
	Vector3( 1.0, -1.0, -1.0), // r b f
	Vector3( 1.0,  1.0, -1.0), // r t f

	Vector3(-1.0,  1.0,  1.0), // l t b
	Vector3(-1.0, -1.0,  1.0), // l b b
	Vector3( 1.0, -1.0,  1.0), // r b b
	Vector3( 1.0,  1.0,  1.0), // r t b
};

static int faces[6][4] = {

	{3, 2, 6, 7}, // right
	{4, 5, 1, 0}, // left
	{4, 0, 3, 7}, // top
	{1, 5, 6, 2}, // bottom
	{7, 6, 5, 4}, // back
	{0, 1, 2, 3}, // front
};


CubeMap::CubeMap(GLsizei aImageSize /* = 256 */)
: imageSize( aImageSize )
{
	cube = new Mesh();
	cube->addVertex( Vector3(-1.0,  1.0, -1.0) );
	cube->addVertex( Vector3(-1.0, -1.0, -1.0) );
	cube->addVertex( Vector3( 1.0, -1.0, -1.0) );
	cube->addVertex( Vector3( 1.0,  1.0, -1.0) );
	cube->addVertex( Vector3(-1.0,  1.0,  1.0) );
	cube->addVertex( Vector3(-1.0, -1.0,  1.0) );
	cube->addVertex( Vector3( 1.0, -1.0,  1.0) );
	cube->addVertex( Vector3( 1.0,  1.0,  1.0) );

	// Front
	cube->addTriangleIndices(0, 1, 2);
	cube->addTriangleIndices(2, 3, 0);

	// Back
	cube->addTriangleIndices(7, 6, 5);
	cube->addTriangleIndices(5, 4, 7);

	// Left
	cube->addTriangleIndices(4, 5, 1);
	cube->addTriangleIndices(1, 0, 4);

	// Right
	cube->addTriangleIndices(3, 2, 6);
	cube->addTriangleIndices(6, 7, 3);
	
	// Top
	cube->addTriangleIndices(4, 0, 3);
	cube->addTriangleIndices(3, 7, 4);

	// Bottom
	cube->addTriangleIndices(1, 5, 6);
	cube->addTriangleIndices(6, 2, 1);
}

CubeMap::~CubeMap()
{
	delete cube;
}

void CubeMap::render()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);   
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	cube->draw();

	////glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D, texName);
	//glBegin(GL_QUADS);

	//	glTexCoord2f(0.0, 1.0); glVertex3f(-2.0, 2.0, -4.0);
	//	glTexCoord2f(0.0, 0.0); glVertex3f(-2.0, -2.0, -4.0);
	//	glTexCoord2f(1.0, 0.0); glVertex3f(2.0, -2.0, -4.0);
	//	glTexCoord2f(1.0, 1.0); glVertex3f(2.0, 2.0, -4.0);
	//	
	//glEnd();
}

void CubeMap::setFaces( float* aPositiveX, float* aNegativeX, float* aPositiveY, float* aNegativeY, 
					   float* aPositiveZ, float* aNegativeZ )
{
	this->positiveX = aPositiveX;
	this->positiveY = aPositiveY;
	this->positiveZ = aPositiveZ;

	this->negativeX = aNegativeX;
	this->negativeY = aNegativeY;
	this->negativeZ = aNegativeZ;

	setupCubeMap();

	//glGenTextures(1, &texName);
	//
	//glBindTexture(GL_TEXTURE_2D, texName);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
	//	GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
	//	GL_NEAREST);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256,
	//	256, 0, GL_RGB, GL_FLOAT, this->negativeZ);
}

void CubeMap::setupCubeMap()
{
	if (positiveX != NULL && positiveY != NULL && positiveZ != NULL &&
		negativeX != NULL && negativeY != NULL && negativeZ != NULL)
	{

		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Specify the 6 sides of the cube
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, GL_RGB, imageSize, 
			imageSize, 0, GL_RGB, GL_FLOAT, positiveX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, 0, GL_RGB, imageSize, 
			imageSize, 0, GL_RGB, GL_FLOAT, negativeX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, 0, GL_RGB, imageSize,
			imageSize, 0, GL_RGB, GL_FLOAT, positiveY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, 0, GL_RGB, imageSize,
			imageSize, 0, GL_RGB, GL_FLOAT, negativeY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, 0, GL_RGB, imageSize,
			imageSize, 0, GL_RGB, GL_FLOAT, positiveZ);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, 0, GL_RGB, imageSize,
			imageSize, 0, GL_RGB, GL_FLOAT, negativeZ);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_EXT); 
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glEnable(GL_TEXTURE_CUBE_MAP_EXT);   
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_NORMALIZE);
		float diffuse[] = { 0.8, 0.8, 1.0, 1.0 };
		glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);

		glEnable(GL_LIGHT0);
		float pos[] = { 0.0, 0.0, 10.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
	}
}