#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "IBLRefraction.h"

IBLRefraction::IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename, GLuint aCubeMapTexId)
: iEta1( aEta1 )
, iEta2( aEta2 )
, iCubeMapTexId( aCubeMapTexId )
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
void IBLRefraction::start()
{
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, iCubeMapTexId);

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
	float eta				= min(iEta2, iEta1) / max(iEta2, iEta1);
	float f					= ((1.0 - eta) * (1.0 - eta)) / ((1.0 + eta) * (1.0 + eta));
	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("reflectionCubeMap", 0);
	iEtaUniform				= new ShaderUniformValue<float>("eta", eta);
	iFUniform				= new ShaderUniformValue<float>("f", f);
	iFresnelPowerUniform	= new ShaderUniformValue<float>("fresnelPower", 15.0);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );
	iShaderProgram->addUniformObject( iEtaUniform );
	iShaderProgram->addUniformObject( iFresnelPowerUniform );
	iShaderProgram->addUniformObject( iFUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}
