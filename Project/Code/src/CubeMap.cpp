#include <gl/glew.h>
#include <gl/glut.h>
#include <stdio.h>
#include "CubeMap.h"

#include "hdrloader.h"

#define SUFFIX_POSX "_posx.hdr"
#define SUFFIX_POSY "_posy.hdr"
#define SUFFIX_POSZ "_posz.hdr"
#define SUFFIX_NEGX "_negx.hdr"
#define SUFFIX_NEGY "_negy.hdr"
#define SUFFIX_NEGZ "_negz.hdr"

static const string suffixes[] = {
	SUFFIX_POSY, 
	SUFFIX_POSZ, 
	SUFFIX_NEGX, 
	SUFFIX_POSX,
	SUFFIX_NEGY,
	SUFFIX_NEGZ
};

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


CubeMap::CubeMap(GLint nChannels, GLsizei aImageSize /* = 256 */)
: imageSize( aImageSize )
, positiveX( NULL )
, positiveY( NULL )
, positiveZ( NULL )
, negativeX( NULL )
, negativeY( NULL )
, negativeZ( NULL )
, channels( nChannels )
{
	if (nChannels == 3)
	{
		this->format = GL_RGB;
	}
	else
	{
		this->format = GL_RGBA;
	}
}

CubeMap::~CubeMap()
{
	delete[] positiveX;
	delete[] positiveY;
	delete[] positiveZ;
	delete[] negativeX;
	delete[] negativeY;
	delete[] negativeZ;
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
	this->positiveX = new float[imageSize * imageSize * channels];
	this->positiveY = new float[imageSize * imageSize * channels];
	this->positiveZ = new float[imageSize * imageSize * channels];
	this->negativeX = new float[imageSize * imageSize * channels];
	this->negativeY = new float[imageSize * imageSize * channels];
	this->negativeZ = new float[imageSize * imageSize * channels];

	int start = 0;
	for (int i = 0; i < imageSize; i++)
	{
		int index	= i * width * channels;
		int index2	= (imageSize - i - 1) * imageSize * channels;

		// Face copy
		memcpy(&(positiveY[index2]), &columns[0][start + index], sizeof(float) * imageSize * channels);
		memcpy(&(positiveZ[index2]), &columns[1][start + index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeX[index2]), &columns[2][start + index], sizeof(float) * imageSize * channels);
		memcpy(&(positiveX[index2]), &columns[3][start + index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeY[index2]), &columns[4][start + index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeZ[index2]), &columns[5][start + index], sizeof(float) * imageSize * channels);
	}
}

void CubeMap::setupCubeMap(string filenamePrefix)
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

	HDRLoaderResult hdrPic;
	float** faces = (float**)malloc(sizeof(int) * 6);

	for (int i = 0; i < 6; i++)
	{
		HDRLoader::load(("./textures/stpeters" + suffixes[i]).c_str(), hdrPic);
		faces[i] = hdrPic.cols;
	}
	

	//// Positive Y
	//HDRLoader::load("./textures/stpeters_posy.hdr", hdrPic);
	//faces[0] = hdrPic.cols;

	//// Positive Z
	//HDRLoader::load("./textures/stpeters_posz.hdr", hdrPic);
	//faces[1] = hdrPic.cols;

	//// Negative X
	//HDRLoader::load("./textures/stpeters_negx.hdr", hdrPic);
	//faces[2] = hdrPic.cols;

	//// Positive X
	//HDRLoader::load("./textures/stpeters_posx.hdr", hdrPic);
	//faces[3] = hdrPic.cols;

	//// Negative Y
	//HDRLoader::load("./textures/stpeters_negy.hdr", hdrPic);
	//faces[4] = hdrPic.cols;

	//// Negative Z	
	//HDRLoader::load("./textures/stpeters_negz.hdr", hdrPic);
	//faces[5] = hdrPic.cols;

	extractFaces(faces, hdrPic.height, hdrPic.width);

	for (int i = 0; i < 6; i++)
	{
		delete []faces[i];
	}



	if (positiveX != NULL && positiveY != NULL && positiveZ != NULL &&
		negativeX != NULL && negativeY != NULL && negativeZ != NULL)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the 6 sides of the cube
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, format, imageSize, 
			imageSize, 0, format, GL_FLOAT, positiveX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, 0, format, imageSize, 
			imageSize, 0, format, GL_FLOAT, negativeX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, positiveY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, negativeY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, positiveZ);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, negativeZ);

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