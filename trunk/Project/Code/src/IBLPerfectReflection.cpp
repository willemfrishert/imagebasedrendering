#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "IBLPerfectReflection.h"

#include "hdrloader.h"

IBLPerfectReflection::IBLPerfectReflection(const string& ashaderFilename, const string& aPanoramaFilename)
{
	setupIrradianceMapTexture( aPanoramaFilename );

	initShaders( ashaderFilename );
}

IBLPerfectReflection::~IBLPerfectReflection(void)
{
}

/**
 *
 */
void IBLPerfectReflection::start()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, iIrradianceMapTexId);

	iShaderProgram->useProgram();
}

/**
 *
 */
void IBLPerfectReflection::stop()
{
	iShaderProgram->disableProgram();
}

/**
 *
 */
void IBLPerfectReflection::setupIrradianceMapTexture(const string& aPanoramaFilename)
{
	HDRLoaderResultEncoded hdrPic;
	HDRLoader::load(aPanoramaFilename.c_str(), hdrPic);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &iIrradianceMapTexId);

	// NOTE: use GL_TEXTURE_RECTANGLE_ARB in order to specify coords
	// from [0, height] x [0, width]
	glBindTexture(GL_TEXTURE_2D, iIrradianceMapTexId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// NOTE: as opposed to what they say in the extension description of 
	// GL_TEXTURE_RECTANGLE_ARB, ATI seems to not support 
	// the GL_REPEAT for wrapping modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, hdrPic.width, 
		hdrPic.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, hdrPic.cols);
}

/**
 *
 */
void IBLPerfectReflection::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iIrradianceMapUniform	= new ShaderUniformValue<int>();

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	// Uniforms
	iIrradianceMapUniform->setValue( 0 );
	iIrradianceMapUniform->setName("irradianceMap");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->addUniformObject( iIrradianceMapUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}