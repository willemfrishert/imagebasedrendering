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
	HDRLoaderResult hdrPic;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, 1024, 
		512, 0, GL_RGB, GL_FLOAT, hdrPic.cols);
}

/**
 *
 */
void IBLPerfectReflection::initShaders(const string& ashaderFilename)
{
	iShaderProgram			= new ShaderProgram();
	iIrradianceMapUniform	= new ShaderUniformValue<int>();

	iFragmentShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename);

	// Uniforms
	iIrradianceMapUniform->setValue( 0 );
	iIrradianceMapUniform->setName("irradianceMap");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( ShaderObject(GL_FRAGMENT_SHADER, "./shader/dummie.frag") );
	iShaderProgram->addUniformObject( iIrradianceMapUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}