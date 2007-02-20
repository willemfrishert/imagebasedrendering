#include <gl/glew.h>
#include <gl/glut.h>
#include <stdio.h>
#include "CubeMap.h"


static float vertices[8][3] = {

	{-1.0,  1.0, -1.0}, // l t f
	{-1.0, -1.0, -1.0}, // l b f
	{ 1.0, -1.0, -1.0}, // r b f
	{ 1.0,  1.0, -1.0}, // r t f

	{-1.0,  1.0,  1.0}, // l t b
	{-1.0, -1.0,  1.0}, // l b b
	{ 1.0, -1.0,  1.0}, // r b b
	{ 1.0,  1.0,  1.0}, // r t b
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

void CubeMap::extractFaces(float *columns, int height, int width)
{
	int side = 256;
	int ch = 3;

	this->positiveX = new float[side * side * ch];
	this->positiveY = new float[side * side * ch];
	this->positiveZ = new float[side * side * ch];
	this->negativeX = new float[side * side * ch];
	this->negativeY = new float[side * side * ch];
	this->negativeZ = new float[side * side * ch];

	int start = side * ch;
	for (int i = 0; i < side; i++)
	{
		int index	= i * width * ch;

		//// flip vertically
		//int index2	= (side - i - 1) * side * ch;

		//// positive Y
		//start = side * ch;
		//memcpy(&((*positiveY)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		//// negative X
		//start = res.width * side * ch;
		//memcpy(&((*negativeX)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		//// positive Z
		//start += side * ch;
		//memcpy(&((*positiveZ)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		//// positive X
		//start += side * ch;
		//memcpy(&((*positiveX)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		//// negative Y
		//start = (res.width * side * 2 + side) * ch;
		//memcpy(&((*negativeY)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		//// negative Z
		//start = (3 * side * res.width + side) * ch;
		//memcpy(&((*negativeZ)[index2]), &res.cols[start + index], sizeof(float) * side * ch);

		int index2	= (side - i - 1) * side * ch;

		// positive Y
		start = side * ch;
		memcpy(&((positiveY)[index2]), &columns[start + index], sizeof(float) * side * ch);

		// positive X
		index2	= i * side * ch;
		start = width * side * ch;
		memcpy(&((positiveX)[index2]), &columns[start + index], sizeof(float) * side * ch);

		// negative Z
		start += side * ch;
		memcpy(&((negativeZ)[index2]), &columns[start + index], sizeof(float) * side * ch);

		// negative X
		start += side * ch;
		memcpy(&((negativeX)[index2]), &columns[start + index], sizeof(float) * side * ch);

		// negative Y
		index2	= (side - i - 1) * side * ch;
		start = (width * side * 2 + side) * ch;
		memcpy(&((negativeY)[index2]), &columns[start + index], sizeof(float) * side * ch);

		// positive Z
		start = (3 * side * width + side) * ch;
		memcpy(&((positiveZ)[index2]), &columns[start + index], sizeof(float) * side * ch);
	}
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

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); 
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