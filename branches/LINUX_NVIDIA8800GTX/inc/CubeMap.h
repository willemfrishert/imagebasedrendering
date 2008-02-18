#pragma once

#include <string>

typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;
typedef unsigned int GLenum;

using namespace std;

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

class CubeMap
{
public:
	CubeMap(GLint nChannels);
	~CubeMap();

	void setupCubeMap(string filenamePrefix);
	void setupCompressedCubeMap( string filenamePrefix );

	void render(float* inverseTransMatrix, float& aCameraAngle );

	inline GLuint getCubeMapId() { return iCubeTexId; };

	// methods
private:

	void extractFaces(float** columns, int height, int width);

	void extractFaces(unsigned char** columns, int height, int width);

	void setupCubeMapTextures(GLenum type, GLenum format);

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

	GLint iInternaFormat;

	GLuint iCubeTexId;
};