#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "IBLPerfectReflection.h"

#include "hdrloader.h"

IBLPerfectReflection::IBLPerfectReflection(const string& ashaderFilename)
{
	initShaders( ashaderFilename );
}

IBLPerfectReflection::~IBLPerfectReflection(void)
{
	delete iShaderProgram;
	delete iCubeMapUniform;

	delete iFragmentShader;
	delete iVertexShader;
}

/**
 *
 */
void IBLPerfectReflection::start(GLuint aCubeMapTexId)
{
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, aCubeMapTexId);

	iShaderProgram->useProgram();
}

/**
 *
 */
void IBLPerfectReflection::stop()
{
	iShaderProgram->disableProgram();

	glDisable(GL_TEXTURE_CUBE_MAP);
}

/**
 *
 */
void IBLPerfectReflection::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("reflectionCubeMap", 0);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}