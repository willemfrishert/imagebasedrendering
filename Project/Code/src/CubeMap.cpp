#include <gl/glew.h>
#include <gl/glut.h>
#include <stdio.h>
#include "CubeMap.h"

static float halfside = 2.0f;

static float vertices[8][3] = {

	{-halfside,  halfside, -halfside}, // l t f
	{-halfside, -halfside, -halfside}, // l b f
	{ halfside, -halfside, -halfside}, // r b f
	{ halfside,  halfside, -halfside}, // r t f

	{-halfside,  halfside,  halfside}, // l t b
	{-halfside, -halfside,  halfside}, // l b b
	{ halfside, -halfside,  halfside}, // r b b
	{ halfside,  halfside,  halfside}, // r t b
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
, positiveX( NULL )
, positiveY( NULL )
, positiveZ( NULL )
, negativeX( NULL )
, negativeY( NULL )
, negativeZ( NULL )
{
	//cube = new Mesh();
	//cube->addVertex( Vector3(-1.0,  1.0, -1.0) );
	//cube->addVertex( Vector3(-1.0, -1.0, -1.0) );
	//cube->addVertex( Vector3( 1.0, -1.0, -1.0) );
	//cube->addVertex( Vector3( 1.0,  1.0, -1.0) );
	//cube->addVertex( Vector3(-1.0,  1.0,  1.0) );
	//cube->addVertex( Vector3(-1.0, -1.0,  1.0) );
	//cube->addVertex( Vector3( 1.0, -1.0,  1.0) );
	//cube->addVertex( Vector3( 1.0,  1.0,  1.0) );

	//// Front
	//cube->addTriangleIndices(0, 1, 2);
	//cube->addTriangleIndices(2, 3, 0);

	//// Back
	//cube->addTriangleIndices(7, 6, 5);
	//cube->addTriangleIndices(5, 4, 7);

	//// Left
	//cube->addTriangleIndices(4, 5, 1);
	//cube->addTriangleIndices(1, 0, 4);

	//// Right
	//cube->addTriangleIndices(3, 2, 6);
	//cube->addTriangleIndices(6, 7, 3);
	//
	//// Top
	//cube->addTriangleIndices(4, 0, 3);
	//cube->addTriangleIndices(3, 7, 4);

	//// Bottom
	//cube->addTriangleIndices(1, 5, 6);
	//cube->addTriangleIndices(6, 2, 1);
}

CubeMap::~CubeMap()
{
	//delete cube;
}

void CubeMap::render( float* inverseTransMatrix )
{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);   
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();// Texture
	glLoadIdentity();
	glMultMatrixf( inverseTransMatrix );

	for (int i = 0; i < 6; ++i) 
	{
		glBegin(GL_QUADS);
		
		for (int j = 0; j < 4; ++j) 
		{
			glNormal3fv( vertices[ faces[i][j] ] );
			glVertex3fv( vertices[ faces[i][j] ] );
		}

		glEnd();

	}

	glPopMatrix();// Texture
	glMatrixMode(GL_MODELVIEW);

	//glEnable(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D, texName);
	//glBegin(GL_QUADS);

	//	glTexCoord2f(0.0, 1.0); glVertex3f(-2.0, 2.0, -4.0);
	//	glTexCoord2f(0.0, 0.0); glVertex3f(-2.0, -2.0, -4.0);
	//	glTexCoord2f(1.0, 0.0); glVertex3f(2.0, -2.0, -4.0);
	//	glTexCoord2f(1.0, 1.0); glVertex3f(2.0, 2.0, -4.0);
	//	
	//	//glVertex3f(-2.0, 2.0, -4.0); glTexCoord2f(0.0, 1.0); 
	//	//glVertex3f(-2.0, -2.0, -4.0); glTexCoord2f(1.0, 1.0); 
	//	//glVertex3f(2.0, -2.0, -4.0); glTexCoord2f(1.0, 0.0); 
	//	//glVertex3f(2.0, 2.0, -4.0); glTexCoord2f(0.0, 0.0);

	//glEnd();
}

void CubeMap::extractFaces(float **columns, int height, int width)
{
	int side = 256;
	int ch = 3;

	this->positiveX = new float[side * side * ch];
	this->positiveY = new float[side * side * ch];
	this->positiveZ = new float[side * side * ch];
	this->negativeX = new float[side * side * ch];
	this->negativeY = new float[side * side * ch];
	this->negativeZ = new float[side * side * ch];

	int start = 0;
	for (int i = 0; i < side; i++)
	{
		int index	= i * width * ch;
		int index2	= (side - i - 1) * side * ch;
		//int index2	= i * side * ch;
		//start = side * ch;

		// Face copy
		memcpy(&(positiveY[index2]), &columns[0][start + index], sizeof(float) * side * ch);
		memcpy(&(positiveZ[index2]), &columns[1][start + index], sizeof(float) * side * ch);
		memcpy(&(negativeX[index2]), &columns[2][start + index], sizeof(float) * side * ch);
		memcpy(&(positiveX[index2]), &columns[3][start + index], sizeof(float) * side * ch);
		memcpy(&(negativeY[index2]), &columns[4][start + index], sizeof(float) * side * ch);
		memcpy(&(negativeZ[index2]), &columns[5][start + index], sizeof(float) * side * ch);
	}
}

void CubeMap::setupCubeMap()
{
	//glGenTextures(1, &texName);
	//glBindTexture(GL_TEXTURE_2D, texName);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
	//	GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
	//	GL_NEAREST);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageSize,
	//	imageSize, 0, GL_RGB, GL_FLOAT, this->negativeX);

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

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); 
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glEnable(GL_TEXTURE_CUBE_MAP_EXT);   
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_NORMALIZE);
	}
}