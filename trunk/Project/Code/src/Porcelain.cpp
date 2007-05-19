#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "Porcelain.h"

#include "hdrloader.h"

Porcelain::Porcelain(float aEta1, float aEta2, const string& ashaderFilename, GLuint aCubeMapTexId, GLuint aDiffuseCubeMapTexId)
: iEta1( aEta1 )
, iEta2( aEta2 )
, iCubeMapTexId(aCubeMapTexId)
, iDiffuseCubeMapTexId(aDiffuseCubeMapTexId)
{
	initShaders( ashaderFilename );
}

Porcelain::~Porcelain(void)
{
	delete iShaderProgram;
	delete iCubeMapUniform;

	delete iFragmentShader;
	delete iVertexShader;
	delete iMaterialReflectance;
	delete iFresnelPower;
}

/**
*
*/
void Porcelain::start()
{
	// enable multitexturing
	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, iCubeMapTexId );

	// enable multitexturing
	glActiveTexture( GL_TEXTURE1 );
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, iDiffuseCubeMapTexId );

	iShaderProgram->useProgram();
}

/**
*
*/
void Porcelain::stop()
{
	iShaderProgram->disableProgram();

	// enable multitexturing
	glActiveTexture( GL_TEXTURE0 );
	glDisable(GL_TEXTURE_CUBE_MAP);

	// enable multitexturing
	glActiveTexture( GL_TEXTURE1 );
	glDisable(GL_TEXTURE_CUBE_MAP);

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
}

/**
*
*/
void Porcelain::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("reflectionCubeMap", 0);
	iDiffuseCubeMapUniform	= new ShaderUniformValue<int>("diffuseCubeMap", 1);

	// See OpenGL Shading Language, Second Edition (orange book) page 338
	float Eta = min(iEta2, iEta1) / max(iEta2, iEta1); // Ratio of indices of refraction
	float fresnelPower = 15.0f;
	float f = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

	iMaterialReflectance	= new ShaderUniformValue<float>("f", f);
	iFresnelPower			= new ShaderUniformValue<float>("fresnelPower", fresnelPower);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );
	iShaderProgram->addUniformObject( iDiffuseCubeMapUniform );
	iShaderProgram->addUniformObject( iMaterialReflectance );
	iShaderProgram->addUniformObject( iFresnelPower );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}