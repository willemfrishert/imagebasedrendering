#include <gl/glew.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "GPUParallelReductor.h"
#include "Utility.h"

#include "PhotographicToneMapper.h"

PhotographicToneMapper::PhotographicToneMapper(GLuint aOriginalTexture, GLuint aLuminanceTexture, 
											   int aWidth, int aHeight)
: iOriginalTexture( aOriginalTexture )
, iLuminanceTexture( aLuminanceTexture )
, iWidth( aWidth )
, iHeight( aHeight )
, iInvalidateExposure( true )
{
	this->iLogAverageCalculator = new GPUParallelReductor(aWidth, aHeight, 
		"./shader/logsumFP.frag", GL_RGBA_FLOAT32_ATI);

	initShaders("./shader/photoToneMapper.frag");
}

PhotographicToneMapper::~PhotographicToneMapper(void)
{
	delete iLogAverageCalculator;
}

/**
 * @param aOriginalTexture
 * @param aLuminanceTexture
 */
void PhotographicToneMapper::toneMap(GLuint aOriginalTexture, GLuint aLuminanceTexture)
{
	float logSum[4];
	iLogAverageCalculator->processData(aLuminanceTexture, logSum);
	float logAverage = exp( (logSum[0] / (float)(iHeight * iWidth)));
	float LogLmin = Utility::log2(logSum[3]);
	float LogLmax = Utility::log2(logSum[2]);
	
	float f = (2 * Utility::log2(logAverage) - LogLmin - LogLmax) / (LogLmax - LogLmin);
	float key = 0.18f * pow(4.0f, f);

	if ( iInvalidateExposure )
	{
		iInvalidateExposure = false;
		iPreviousExposure = 1.0;
	}

	float correctExposure = log(logAverage / key);
	Utility::clamp(correctExposure, 0.18f, correctExposure);
	float ratio = 0.05;
	float newExposure = iPreviousExposure + (correctExposure - iPreviousExposure) * ratio;
	iPreviousExposure = newExposure;

	//printf("Key: %f\t\tLogAverage: %f\n", key, logAverage);
	//printf("Lavg: %f\t\tScale: %f\n", logAverage, scale);
	//printf("CORRECT: %f\t\CURRENT: %f\t\t NEW: %f\n", correctExposure, iPreviousExposure, newExposure);

	iLogAverageUniform->setValue( logAverage );
	iExposureUniform->setValue( newExposure );

	iShaderProgram->useProgram();
	{
		renderSceneOnQuad(aOriginalTexture, aLuminanceTexture);
	}
	iShaderProgram->disableProgram();
}

/**
 * @param aValue the value of exposure; it will be clamped to [0, 1]
 */
void PhotographicToneMapper::setExposure(float aValue)
{
	aValue = aValue > 1.0 ? 1.0 : aValue;
	aValue = aValue < 0.0 ? 1e-10 : aValue;
	this->iExposureUniform->setValue( aValue );
}

/**
 * @return the value of the exposure
 */
float PhotographicToneMapper::getExposure()
{
	return this->iExposureUniform->getValue();
}


/**
 * @description invalidates the exposure so the next
 * frame exposure will be set as the default.
 */
void PhotographicToneMapper::InvalidateExposure()
{
	//this->iInvalidateExposure = true;
}

/************************************************************************/
/* ****************** PRIVATE METHODS **************                    */
/************************************************************************/

void PhotographicToneMapper::enableMultitexturing( GLuint aOriginalTexture,  GLuint aLuminanceTexture )
{
	// enable multitexturing
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aOriginalTexture);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aLuminanceTexture);
}

void PhotographicToneMapper::disableMultitexturing()
{
	// GL_TEXTURE1: Luminance Texture
	glActiveTexture( GL_TEXTURE1 );
	glDisable(GL_TEXTURE_2D);

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	//glDisable(GL_TEXTURE_2D);
}

void PhotographicToneMapper::renderSceneOnQuad(GLuint aOriginalTexture, GLuint aLuminanceTexture)
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	{
		glLoadIdentity();
		gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f);
	
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		{
			glLoadIdentity();

			// setup flags and bind textures
			enableMultitexturing(aOriginalTexture, aLuminanceTexture);

			glBegin(GL_QUADS);
		
			glTexCoord2f(0.0f, 0.0f );
			glVertex2f(0.0, 0.0);

			glTexCoord2f(1.0f, 0.0f );
			glVertex2f(1.0f, 0.0);

			glTexCoord2f(1.0f, 1.0f );
			glVertex2f(1.0f, 1.0f);

			glTexCoord2f(0.0f, 1.0f );
			glVertex2f(0.0, 1.0f);
		
			glEnd();
		} //End GL_PROJECTION

		glPopMatrix();
	
		glMatrixMode( GL_PROJECTION );
	}
	glPopMatrix(); // End GL_PROJECTION

	glMatrixMode( GL_MODELVIEW );
	
	disableMultitexturing();
}

void PhotographicToneMapper::initShaders( string fragmentShaderFilename )
{
	iShaderProgram				= new ShaderProgram();
	iOriginalTextureUniform		= new ShaderUniformValue<int>();
	iLuminanceTextureUniform	= new ShaderUniformValue<int>();
	iLogAverageUniform			= new ShaderUniformValue<float>();
	iExposureUniform			= new ShaderUniformValue<float>();

	iFragmentShader				= new ShaderObject(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	iOriginalTextureUniform->setValue( 0 );
	iOriginalTextureUniform->setName("originalTex");
	
	iLuminanceTextureUniform->setValue( 1 );
	iLuminanceTextureUniform->setName("luminanceTex");
	
	iLogAverageUniform->setValue( 0.0f );
	iLogAverageUniform->setName("logAverage");

	iExposureUniform->setValue( 1.0f );
	iExposureUniform->setName("exposure");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->addUniformObject( iOriginalTextureUniform );
	iShaderProgram->addUniformObject( iLuminanceTextureUniform );
	iShaderProgram->addUniformObject( iLogAverageUniform );
	iShaderProgram->addUniformObject( iExposureUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}