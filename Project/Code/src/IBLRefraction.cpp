#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "IBLRefraction.h"

IBLRefraction::IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename)
: iEta1( aEta1 )
, iEta2( aEta2 )
{
	initShaders( ashaderFilename );
}

IBLRefraction::~IBLRefraction(void)
{
	delete iShaderProgram;
	delete iCubeMapUniform;

	delete iFragmentShader;
	delete iVertexShader;
}

/**
*
*/
void IBLRefraction::start(GLuint aCubeMapTexId)
{
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, aCubeMapTexId);

	iShaderProgram->useProgram();
}

/**
*
*/
void IBLRefraction::stop()
{
	iShaderProgram->disableProgram();

	glDisable(GL_TEXTURE_CUBE_MAP);
}

/**
 * @param ashaderFilename
 */
void IBLRefraction::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("reflectionCubeMap", 0);
	iEta1Uniform			= new ShaderUniformValue<float>("eta1", 1.5f);
	iEta2Uniform			= new ShaderUniformValue<float>("eta2", 1.0f);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );
	iShaderProgram->addUniformObject( iEta1Uniform );
	iShaderProgram->addUniformObject( iEta2Uniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}
