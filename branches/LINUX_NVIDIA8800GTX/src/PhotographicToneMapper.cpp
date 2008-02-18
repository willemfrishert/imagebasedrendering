#include <GL/glew.h>
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
, iBaseExposure( 1.0 )
{
  //	this->iLogAverageCalculator = new GPUParallelReductor(aWidth, aHeight, 
  //		"./shader/logsumFP.frag", GL_RGBA_FLOAT32_ATI);

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
void PhotographicToneMapper::ToneMap(GLuint aOriginalTexture, GLuint aLuminanceTexture, float* logStatistics)
{
	// Compute Log information: LOG AVERAGE, MIN and MAX 
	//float logStatistics[4];
	//iLogAverageCalculator->processData(aLuminanceTexture, logStatistics);
	float logAverage = exp( (logStatistics[0] / (float)(iHeight * iWidth)));
 	
	//printf("Lavg: %f\t\tScale: %f\n", logAverage, scale);
	//printf("CORRECT: %f\t\CURRENT: %f\t\t NEW: %f\n", correctExposure, iPreviousExposure, newExposure);
	//printf("LOGAVERAGE: %f\t\logMin: %f\t\t LogMax: %f\n", logAverage, logStatistics[3], logStatistics[2]);

	float currentExposure = computeCurrentExposure(logAverage, logStatistics[3], logStatistics[2]);
	//printf("LogAverage: %f\t\tMinExposure: %f\t\tMaxExposure: %f\t\tcurrent Exposure: %f\n", logAverage,  logStatistics[3], logStatistics[2], currentExposure);

	iExposureUniform->setValue( currentExposure );
	//	printf("Exposure: %f\n", currentExposure);
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
	//aValue = aValue > 1.0 ? 1.0 : aValue;
	aValue = aValue < 0.0 ? 1e-10 : aValue;
	this->iExposureUniform->setValue( aValue );

	printf("Exposure Set: %f\n", aValue);
}

/**
* @return the value of the exposure
*/
float PhotographicToneMapper::getExposure()
{
	return this->iExposureUniform->getValue();
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
	float Log2LMin = Utility::log2(aMinLuminance + 1e-8);
	float Log2LMax = Utility::log2(aMaxLuminance + 1e-8);

#ifdef _DEBUG
	assert(aLogLAverage >= 1e-8);
#endif // _DEBUG

	float f = (2 * Utility::log2(aLogLAverage) - Log2LMin - Log2LMax) / (Log2LMax - Log2LMin);
	float key = 1.2f * pow(4.0f, f);

	static int frameCount = 0;

	float correctExposure = key / aLogLAverage;
	Utility::clamp(correctExposure, 0.1f, correctExposure);

	// store the correct as the base every 0.5 sec
	if ( frameCount == 0 )
	{
		iBaseExposure = correctExposure;
	}

	float deltaExposure = (correctExposure + iBaseExposure) / 2.0f - iPreviousExposure;
	float ratio = 0.1f;

	//std::cout << "ratio: " << ratio << "\t\tDELTA: " << deltaExposure << std::endl;

	float currentExposure = iPreviousExposure + deltaExposure * ratio;

	//printf("CORRECT: %f\t\CURRENT: %f\t\t NEW: %f\n", correctExposure, iPreviousExposure, currentExposure);
	//printf("LOG AVERAGE: %f\t MAX: %f\t MIN: %f \tKEY: %f\n", aLogLAverage, aMaxLuminance, aMinLuminance, key);

	iPreviousExposure = currentExposure;

	// update frameCount
	frameCount = (frameCount + 1) % 25;

	return iPreviousExposure;
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
	iExposureUniform			= new ShaderUniformValue<float>();

	iFragmentShader				= new ShaderObject(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	// Initialize Uniform Variables
	iOriginalTextureUniform->setValue( 0 );
	iOriginalTextureUniform->setName("originalTex");
	
	iExposureUniform->setValue( 1.0f );
	iExposureUniform->setName("exposure");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iOriginalTextureUniform );
	iShaderProgram->addUniformObject( iExposureUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}
