#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "Diffuse.h"

#include "hdrloader.h"

Diffuse::Diffuse( const string& ashaderFilename, GLuint aCubeMapTexId )
: iCubeMapTexId( aCubeMapTexId )
{
	initShaders( ashaderFilename );
}

Diffuse::~Diffuse(void)
{
	delete iShaderProgram;
	delete iCubeMapUniform;

	delete iFragmentShader;
	delete iVertexShader;
}

/**
*
*/
void Diffuse::start()
{
	// enable multitexturing
	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, iCubeMapTexId );

	iShaderProgram->useProgram();
}

void Diffuse::setDiffuseConvCubeMap( GLuint aCubeMapTexId )
{
	this->iCubeMapTexId = aCubeMapTexId;
}

/**
*
*/
void Diffuse::stop()
{
	iShaderProgram->disableProgram();

	// disable multitexturing
	glActiveTexture( GL_TEXTURE0 );
	glDisable(GL_TEXTURE_CUBE_MAP);

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
}

/**
*
*/
void Diffuse::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("diffuseCubeMap", 0);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}