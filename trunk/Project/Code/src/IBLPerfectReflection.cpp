#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "IBLPerfectReflection.h"

#include "hdrloader.h"

IBLPerfectReflection::IBLPerfectReflection(const string& ashaderFilename, const string& aPanoramaFilename)
{
	//setupIrradianceMapTexture( aPanoramaFilename );

	initShaders( ashaderFilename );
}

IBLPerfectReflection::~IBLPerfectReflection(void)
{
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
	iCubeMapUniform	= new ShaderUniformValue<int>();

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	// Uniforms
	iCubeMapUniform->setValue( 0 );
	iCubeMapUniform->setName("reflectionCubeMap");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}