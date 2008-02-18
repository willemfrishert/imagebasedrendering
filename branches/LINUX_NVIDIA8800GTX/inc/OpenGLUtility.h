#pragma once

#include "Basic.h"
#include <vector>

using namespace std;

#ifdef _DEBUG
#define CHECK_GL_ERROR() OpenGLUtility::CheckGLError(__FILE__, __LINE__)
#else
#define CHECK_GL_ERROR()
#endif

class OpenGLUtility
{
public:
	static void RenderSceneOnQuadMultiTex(const GLuint* aTextureId, int aNumberOfTextures);
	
	// GL ERROR CHECK
	static inline int CheckGLError(char *aFile, int aLine);

private:
	static void EnableMultitexturing( const GLuint* aTextureId, const GLenum* aTextureUnit, int aNumberOfTextures );
	static void DisableMultitexturing( const GLenum* aTextureUnit, int aNumberOfTextures );
};

inline int OpenGLUtility::CheckGLError(char *aFile, int aLine)
{
	int result = 0;
	GLenum glError = glGetError();

	while (glError != GL_NO_ERROR)
	{
		cout << "Error in file " << aFile << " at line: " << aLine << endl;
		cout << "GL Error #" << glError << ": " << gluErrorString(glError) << endl << endl;
		result = 1;
		glError = glGetError();
	}

	return result;
}


