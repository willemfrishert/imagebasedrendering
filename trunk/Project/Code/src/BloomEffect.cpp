#include "BloomEffect.h"
#include "Utility.h"

BloomEffect::BloomEffect()
: iOriginalImageSize( 512 )
, iBrightpassToggle(0)
{
	Init();
}

BloomEffect::~BloomEffect()
{
	delete iOriginalFBO;
	delete iBlendedFBO;

	delete iBrightpassShaderProgram;
	delete iBlenderShaderProgram;

	delete iCodecRGBEFragmentShader;
	delete iBrightpassFragmentShader;

	delete iLuminanceConverter;
	delete iGPUParallelReductor;
	delete iBlurTexture;

	glDeleteTextures(1, &iOriginalTexture);
	glDeleteTextures(2, iBrightpassTexture);
	glDeleteTextures(1, &iBlendedTexture);
}

void BloomEffect::Init()
{
	iLuminanceConverter = new LuminanceConverter(iOriginalImageSize, iOriginalImageSize, -1, -1);
	iGPUParallelReductor = new GPUParallelReductor(iOriginalImageSize, iOriginalImageSize, "./shader/logsumFP.frag");

	InitFramebufferObject();
	InitTextures();
	InitShaders();

	iBlurTexture = new BlurTexture();
}

void BloomEffect::InitTextures()
{
	iOriginalFBO->bind();

	iOriginalFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexture);

	glBindTexture(GL_TEXTURE_2D, iOriginalTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	iOriginalFBO->attachTexture(iOriginalTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	// initialize texture that will store the framebuffer image
	glGenTextures(2, iBrightpassTexture);

	iBrightpassFBOAttachment[0] = GL_COLOR_ATTACHMENT1_EXT;
	iBrightpassFBOAttachment[1] = GL_COLOR_ATTACHMENT2_EXT;

	for (int i=0; i<2; i++)
	{	
		glBindTexture(GL_TEXTURE_2D, iBrightpassTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

		iOriginalFBO->attachTexture(iBrightpassTexture[i], iBrightpassFBOAttachment[i], GL_TEXTURE_2D, 0);
	}

	FrameBufferObject::unbind();

	iBlendedFBO->bind();
	iBlendedFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	glGenTextures(1, &iBlendedTexture);

	glBindTexture(GL_TEXTURE_2D, iBlendedTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);
	iBlendedFBO->attachTexture(iBlendedTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	FrameBufferObject::unbind();
}

void BloomEffect::InitFramebufferObject()
{
	iOriginalFBO = new FrameBufferObject();	
	iBlendedFBO = new FrameBufferObject();
}

/*
* Initializes the program shaders	
*/
void BloomEffect::InitShaders()
{
	InitCodecShaderObject();
	InitBrightpassShader();
	InitBlendShader();
}

void BloomEffect::InitCodecShaderObject()
{
	iCodecRGBEFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/codecRGBE.frag");
}

void BloomEffect::InitBrightpassShader()
{
	iBrightpassShaderProgram = new ShaderProgram();
	iBrightpassFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/brightpass.frag");
	iBrightpassShaderProgram->attachShader( *iBrightpassFragmentShader );
	iBrightpassShaderProgram->attachShader( *iCodecRGBEFragmentShader );

	iLogAvgLum.setValue(0.0f);
	iLogAvgLum.setName("logAvgLum");

	iPrevBrightpassTextureUniform.setValue(1);
	iPrevBrightpassTextureUniform.setName("prevBrightpass");

	iBrightpassShaderProgram->addUniformObject( &iLogAvgLum );
	iBrightpassShaderProgram->addUniformObject( &iPrevBrightpassTextureUniform );
	iBrightpassShaderProgram->buildProgram();
}

void BloomEffect::InitBlendShader()
{
	iBlenderFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/bloomBlender.frag");
	iBlenderShaderProgram = new ShaderProgram();

	iBlenderShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iBlenderShaderProgram->attachShader( *iBlenderFragmentShader );

	iBlenderOriginalTextureUniform.setValue( 0 );
	iBlenderOriginalTextureUniform.setName("originalTexture");

	iBlenderBlur1TextureUniform.setValue( 1 );
	iBlenderBlur1TextureUniform.setName("blurTexture0");

	iBlenderBlur2TextureUniform.setValue( 2 );
	iBlenderBlur2TextureUniform.setName("blurTexture1");

	iBlenderBlur3TextureUniform.setValue( 3 );
	iBlenderBlur3TextureUniform.setName("blurTexture2");

	iBlenderBlur4TextureUniform.setValue( 4 );
	iBlenderBlur4TextureUniform.setName("blurTexture3");

	iBlenderShaderProgram->addUniformObject( &iBlenderOriginalTextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur1TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur2TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur3TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur4TextureUniform );

	iBlenderShaderProgram->buildProgram();
}

void BloomEffect::Begin()
{
	// ******* RENDER TO THE FRAMEBUFFER ********
	//bind the FBO, and the associated texture.
	iOriginalFBO->bind();
	//draw to original texture
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glGetIntegerv( GL_VIEWPORT, iOldViewPort );	

	glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );
	
	// #### FIRST STEP: Draw the object into the original texture: 'originalTexture'
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/************************************************************************/
	/* ######################### DRAW AFTER THIS !!!                        */
	/************************************************************************/
}



GLuint BloomEffect::End()
{
	/************************************************************************/
	/* ######################### DRAW WAS DONE BEFORE THIS !!!              */
	/************************************************************************/

	FrameBufferObject::unbind();

	GLuint luminanceTexture = iLuminanceConverter->processData(iOriginalTexture);
	iGPUParallelReductor->processData(luminanceTexture, logSum);
	float logAverage = exp( (logSum[0] / (float)(iOriginalImageSize * iOriginalImageSize)));

	static float iPreviousExposure = 1;
	//float logSum[4];
	//iLogAverageCalculator->processData(aLuminanceTexture, logSum);
	//float logAverage = exp( (logSum[0] / (float)(iHeight * iWidth)));
	float LogLmin = Utility::log2(logSum[3]);
	float LogLmax = Utility::log2(logSum[2]);

	float f = (2 * Utility::log2(logAverage) - LogLmin - LogLmax) / (LogLmax - LogLmin);
	float key = 0.18f * pow(4.0f, f);

	//if ( iInvalidateExposure )
	//{
	//	iInvalidateExposure = false;
	//	iPreviousExposure = 1.0;
	//}

	float correctExposure = log(logAverage / key);
	Utility::clamp(correctExposure, 0.18f, correctExposure);
	float ratio = 0.05;
	float newExposure = iPreviousExposure + (correctExposure - iPreviousExposure) * ratio;
	iPreviousExposure = newExposure;

	iOriginalFBO->bind();


	static bool iFirstTime = true;
	glDrawBuffer(iBrightpassFBOAttachment[(iBrightpassToggle+1)%2]);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////////glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );
	iLogAvgLum.setValue( logAverage );
	iBrightpassShaderProgram->useProgram();
	if (iFirstTime)
	{
		RenderSceneOnQuadMultiTex( iOriginalTexture, iOriginalTexture );
		iFirstTime = false;
	}
	else
	{
		RenderSceneOnQuadMultiTex( iOriginalTexture, iBrightpassTexture[iBrightpassToggle] );
	}
	iBrightpassShaderProgram->disableProgram();

	//const int width = 4;
	//const int height = 4;
	//float result[width*height*4];
	//glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
	//glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, result);
	FrameBufferObject::unbind();

	iBrightpassToggle = (iBrightpassToggle+1)%2;
	iFinalBlurredTexture = iBlurTexture->processData(iBrightpassTexture[iBrightpassToggle]);
	

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//RenderSceneOnQuad(iFinalBlurredTexture[0]);
	RenderBloomEffect(iOriginalTexture, iFinalBlurredTexture);

	//return iBrightpassTexture[iBrightpassToggle];
	//return iFinalBlurredTexture[2];
	//return iFinalBlurredTexture[2];
	//return iFinalBlurredTexture[0];
	return iBlendedTexture;
}


void BloomEffect::RenderSceneOnQuad( GLuint aTextureID )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( -1.0f, 1.0f, -1.0f, 1.0f );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, aTextureID );

	glBegin(GL_QUADS);
	glTexCoord2f( 0, 0 );
	glVertex2f( -1, -1 );

	glTexCoord2f( 1, 0 );
	glVertex2f( 1, -1 );

	glTexCoord2f( 1, 1 );
	glVertex2f( 1, 1 );

	glTexCoord2f( 0, 1 );
	glVertex2f( -1, 1 );
	glEnd();

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}

void BloomEffect::RenderBloomEffect( GLuint aOriginalTexture, GLuint* aBlurredTextures )
{
	iBlendedFBO->bind();
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iBlenderShaderProgram->useProgram();
		RenderSceneOnQuad(aOriginalTexture, aBlurredTextures );
	iBlenderShaderProgram->disableProgram();
	FrameBufferObject::unbind();
}

void BloomEffect::RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aBlurredTextures )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( -1.0f, 1.0f, -1.0f, 1.0f );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, aOriginalTexture );

	// SETUP MULTITEXTURING
	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		glActiveTexture( GL_TEXTURE1 + i );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, aBlurredTextures[i] );
	}

	//RENDER MULTITEXTURE ON QUAD
	glBegin(GL_QUADS);
	
		for (int i = 0; i < KNumberOfBlurLevels + 1; i++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + i, 0, 0 );
		}
		glVertex2f( -1.0f, -1.0f );

		for (int i = 0; i < KNumberOfBlurLevels + 1; i++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + i, 1, 0 );
		}
		glVertex2f( 1.0f, -1.0f );

		for (int i = 0; i < KNumberOfBlurLevels + 1; i++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + i, 1, 1 );
		}
		glVertex2f( 1.0f, 1.0f );

		for (int i = 0; i < KNumberOfBlurLevels + 1; i++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + i, 0, 1 );
		}
		glVertex2f( -1.0f, 1.0f );
	glEnd();

	// CLEANING MULTITEXTURING
	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		glActiveTexture( GL_TEXTURE1 + i );
		glDisable( GL_TEXTURE_2D );
	}

	glActiveTexture( GL_TEXTURE0 );
	glDisable( GL_TEXTURE_2D );

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}


void BloomEffect::EnableMultitexturing( GLuint aOriginalTexture,  GLuint aLuminanceTexture )
{
	// enable multitexturing
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aOriginalTexture);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aLuminanceTexture);
}

void BloomEffect::DisableMultitexturing()
{
	// GL_TEXTURE1: Luminance Texture
	glActiveTexture( GL_TEXTURE1 );
	glDisable(GL_TEXTURE_2D);

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	//glDisable(GL_TEXTURE_2D);
}

void BloomEffect::RenderSceneOnQuadMultiTex(GLuint aOriginalTexture, GLuint aLuminanceTexture)
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
			EnableMultitexturing(aOriginalTexture, aLuminanceTexture);

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

	DisableMultitexturing();
}
