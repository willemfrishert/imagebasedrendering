#include <gl/glew.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "GPUParallelReductor.h"
#include "Utility.h"
#include "CodecRGBE.h"

#include "PhotographicToneMapper.h"

PhotographicToneMapper::PhotographicToneMapper(GLuint aOriginalTexture, GLuint aLuminanceTexture, 
											   int aWidth, int aHeight)
: iOriginalTexture( aOriginalTexture )
, iLuminanceTexture( aLuminanceTexture )
, iWidth( aWidth )
, iHeight( aHeight )
, iPreviousExposure( 1.0 )
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
	// Compute Log information: LOG AVERAGE, LOG MIN and LOG MAX 
	float logSum[4];
	iLogAverageCalculator->processData(aLuminanceTexture, logSum);
	float logAverage = exp( (logSum[0] / (float)(iHeight * iWidth)));
	
	//printf("Key: %f\t\tLogAverage: %f\t\tPre-Exposure: %f\n", key, logAverage, correctExposure);
	//printf("Lavg: %f\t\tScale: %f\n", logAverage, scale);
	//printf("CORRECT: %f\t\CURRENT: %f\t\t NEW: %f\n", correctExposure, iPreviousExposure, newExposure);

	float currentExposure = computeCurrentExposure(logAverage, logSum[3], logSum[2]);

	iLogAverageUniform->setValue( logAverage );
	iExposureUniform->setValue( currentExposure );

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

/**
* @param aLogLAverage the log luminance average
* @param aLogLMin the log luminance minimum
* @param aLogLMax the log luminance maximum
* @return the current scene exposure
*/
float PhotographicToneMapper::computeCurrentExposure( float aLogLAverage, float aMinLuminance, float aMaxLuminance )
{
	// Key of the Scene: the log2(log(Luminance)) is on purpose
	float Log2LMin = Utility::log2(aMinLuminance);
	float Log2LMax = Utility::log2(aMaxLuminance);
	float f = (2 * Utility::log2(aLogLAverage) - Log2LMin - Log2LMax) / (Log2LMax - Log2LMin);
	float key = 1.0f * pow(4.0f, f);

	/*float correctExposure = log(aLogLAverage / key);*/
	
	float correctExposure = key;
	Utility::clamp(correctExposure, 0.18f, correctExposure);
	
	float deltaExposure = correctExposure - iPreviousExposure;

	/*float ratio = exp(-abs(deltaExposure) * 100.0f);*/
	
	float ratio = 0.03;
	
	//std::cout << "ratio: " << ratio << "\t\tDELTA: " << deltaExposure << std::endl;
	
	float currentExposure = iPreviousExposure + deltaExposure * ratio;

	//printf("CORRECT: %f\t\CURRENT: %f\t\t NEW: %f\n", correctExposure, iPreviousExposure, currentExposure);
	//printf("LOG AVERAGE: %f\t\t\ MAX: %f\t\t MIN: %f\n", aLogLAverage, aMaxLuminance, aMinLuminance);

	iPreviousExposure = currentExposure;

	return currentExposure;
}

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

	// Initialize Uniform Variables
	iOriginalTextureUniform->setValue( 0 );
	iOriginalTextureUniform->setName("originalTex");
	
	iLuminanceTextureUniform->setValue( 1 );
	iLuminanceTextureUniform->setName("luminanceTex");
	
	iLogAverageUniform->setValue( 0.0f );
	iLogAverageUniform->setName("logAverage");

	iExposureUniform->setValue( 1.0f );
	iExposureUniform->setName("exposure");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iOriginalTextureUniform );
	iShaderProgram->addUniformObject( iLuminanceTextureUniform );
	iShaderProgram->addUniformObject( iLogAverageUniform );
	iShaderProgram->addUniformObject( iExposureUniform );
	//iShaderProgram->addUniformObject( iSensitivityUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}