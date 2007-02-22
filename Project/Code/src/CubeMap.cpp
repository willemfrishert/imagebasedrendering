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

static float vertices[8][3] = {

	{-1.0f,  1.0f, -1.0f}, // l t f
	{-1.0f, -1.0f, -1.0f}, // l b f
	{ 1.0f, -1.0f, -1.0f}, // r b f
	{ 1.0f,  1.0f, -1.0f}, // r t f

	{-1.0f,  1.0f,  1.0f}, // l t b
	{-1.0f, -1.0f,  1.0f}, // l b b
	{ 1.0f, -1.0f,  1.0f}, // r b b
	{ 1.0f,  1.0f,  1.0f}, // r t b
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
	glEnable(GL_TEXTURE_CUBE_MAP);   
	glEnable(GL_AUTO_NORMAL);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); // Modelview
	{
		glLoadIdentity();

		glMatrixMode(GL_TEXTURE);
		glPushMatrix(); // Texture
		{
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
		}
		glPopMatrix(); // Texture

		glMatrixMode(GL_MODELVIEW);
	}
	glPopMatrix(); // Modelview
}

void CubeMap::extractFaces(float **columns, int height, int width)
{
	this->positiveX = new float[imageSize * imageSize * channels];
	this->positiveY = new float[imageSize * imageSize * channels];
	this->positiveZ = new float[imageSize * imageSize * channels];
	this->negativeX = new float[imageSize * imageSize * channels];
	this->negativeY = new float[imageSize * imageSize * channels];
	this->negativeZ = new float[imageSize * imageSize * channels];

	for (int i = 0; i < imageSize; i++)
	{
		int index	= i * width * channels;
		int index2	= (imageSize - i - 1) * imageSize * channels;

		// Face copy
		memcpy(&(positiveY[index2]), &columns[0][index], sizeof(float) * imageSize * channels);
		memcpy(&(positiveZ[index2]), &columns[1][index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeX[index2]), &columns[2][index], sizeof(float) * imageSize * channels);
		memcpy(&(positiveX[index2]), &columns[3][index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeY[index2]), &columns[4][index], sizeof(float) * imageSize * channels);
		memcpy(&(negativeZ[index2]), &columns[5][index], sizeof(float) * imageSize * channels);
	}
}

void CubeMap::setupCubeMap(string filenamePrefix)
{
	HDRLoaderResult hdrPic;
	float** faces = (float**)malloc(sizeof(int) * 6);

	for (int i = 0; i < 6; i++)
	{
		HDRLoader::load((filenamePrefix + suffixes[i]).c_str(), hdrPic);
		faces[i] = hdrPic.cols;
	}

	extractFaces(faces, hdrPic.height, hdrPic.width);

	for (int i = 0; i < 6; i++)
	{
		delete []faces[i];
	}

	if (positiveX != NULL && positiveY != NULL && positiveZ != NULL &&
		negativeX != NULL && negativeY != NULL && negativeZ != NULL)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the 6 sides of the cube
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, imageSize, 
			imageSize, 0, format, GL_FLOAT, positiveX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, imageSize, 
			imageSize, 0, format, GL_FLOAT, negativeX);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, positiveY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, negativeY);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, positiveZ);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, imageSize,
			imageSize, 0, format, GL_FLOAT, negativeZ);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); 
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glEnable(GL_TEXTURE_CUBE_MAP);   
		glEnable(GL_AUTO_NORMAL);
	}
}