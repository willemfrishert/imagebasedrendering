#include "BloomEffect.h"

BloomEffect::BloomEffect()
: iOriginalImageSize( ETextureSize512 )
//, iImageWinHeight( height )
//, iImageBlurWidth( 16 )
//, iImageBlurHeight( 16 )
{
	Init();
}

BloomEffect::~BloomEffect()
{
	delete iOriginalFBO;
	//delete iUpscaleBlurFBO;

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		delete iIntermediateFBO[i];
	}
}

void BloomEffect::Init()
{
	iLuminanceConverter = new LuminanceConverter(iOriginalImageSize, iOriginalImageSize, -1, -1);
	iGPUParallelReductor = new GPUParallelReductor(iOriginalImageSize, iOriginalImageSize, "./shader/logsumFP.frag");

	InitFramebufferObject();
	InitTextures();
	InitShaders();
}

void BloomEffect::InitTextures()
{
	iMipmapSize[0] = ETextureSize128;
	iMipmapSize[1] = ETextureSize64;
	iMipmapSize[2] = ETextureSize32;
	iMipmapSize[3] = ETextureSize16;

	iOriginalFBO->bind();

	iOriginalFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexture);

	glBindTexture(GL_TEXTURE_2D, iOriginalTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	iOriginalFBO->attachTexture(iOriginalTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iBrightpassTexture);

	glBindTexture(GL_TEXTURE_2D, iBrightpassTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	iOriginalFBO->attachTexture(iBrightpassTexture, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);

	FrameBufferObject::unbind();


	// create a texture for the horizontal blur pass
	glGenTextures(KNumberOfBlurLevels, iHorizBlurredTexture);
	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurLevels, iFinalBlurredTexture);

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i]->bind();

		glBindTexture(GL_TEXTURE_2D, iHorizBlurredTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		//iIntermediateFBO->attachDepthBuffer( depthBufferId );
		iIntermediateFBO[i]->attachTexture(iHorizBlurredTexture[i], GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);
		iIntermediateFBO[i]->attachTexture(iFinalBlurredTexture[i], GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);

		FrameBufferObject::unbind();
	}

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
	
	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i] = new FrameBufferObject();
	}

	//iUpscaleBlurFBO = new FrameBufferObject();

	iBlendedFBO = new FrameBufferObject();
}

/*
* Initializes the program shaders	
*/
void BloomEffect::InitShaders()
{
	InitCodecShaderObject();
	InitBrightpassShader();
	InitBlurShaders();
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

	iBrightpassShaderProgram->addUniformObject( &iLogAvgLum );
	iBrightpassShaderProgram->buildProgram();
}

void BloomEffect::InitBlurShaders()
{
	iHorizontalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur.frag");
	//iHorizontalBlurVertexShader = new ShaderObject(GL_VERTEX_SHADER, "./shader/horizBlur.vert");

	iVerticalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur.frag");
	
	//generate blurred images using the mipmaps
	TMipMapLevel mipmapLevel = EMipMapLevel128;
	TTextureID intermediateTextureNumber = ETextureId128;

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	iHorizontalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iHorizontalShaderProgram->attachShader( *(iHorizontalBlurFragmentShader) );
	//iHorizontalShaderProgram->attachShader( *(iHorizontalBlurVertexShader) );

	iVerticalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iVerticalShaderProgram->attachShader( *(iVerticalBlurFragmentShader) );

	iTextureOriginalUniform.setValue( 0 );
	iTextureOriginalUniform.setName("originalTexture");

	iTextureHorizontalUniform.setValue( 0 );
	iTextureHorizontalUniform.setName("originalTexture");

	iMipmapSizeUniform.setValue( iMipmapSize[0] );
	iMipmapSizeUniform.setName("mipmapSize");

	iMipmapLevelUniform.setValue( mipmapLevel );
	iMipmapLevelUniform.setName("mipmapLevel");

	iHorizTextureSizeUniform.setValue( iMipmapSize[0] );
	iHorizTextureSizeUniform.setName("mipmapSize");

	iHorizontalShaderProgram->addUniformObject( &iTextureOriginalUniform );
	iHorizontalShaderProgram->addUniformObject( &iMipmapSizeUniform );
	iHorizontalShaderProgram->addUniformObject( &iMipmapLevelUniform );
	iVerticalShaderProgram->addUniformObject( &iTextureHorizontalUniform );
	iVerticalShaderProgram->addUniformObject( &iHorizTextureSizeUniform );

	iHorizBlurWeight1Uniform.setName("blurWeight1");
	iHorizBlurWeight2Uniform.setName("blurWeight2");
	iHorizBlurWeight3Uniform.setName("blurWeight3");

	iVertBlurWeight1Uniform.setName("blurWeight1");
	iVertBlurWeight2Uniform.setName("blurWeight2");
	iVertBlurWeight3Uniform.setName("blurWeight3");

	iHorizBlurWeight1Uniform.setValue( 0.375f );
	iHorizBlurWeight2Uniform.setValue( 0.250f );
	iHorizBlurWeight3Uniform.setValue( 0.062f );

	iVertBlurWeight1Uniform.setValue( 0.375f );
	iVertBlurWeight2Uniform.setValue( 0.250f );
	iVertBlurWeight3Uniform.setValue( 0.062f );

	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight1Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight2Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight3Uniform);

	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight1Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight2Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight3Uniform);

	iHorizontalShaderProgram->buildProgram();
	iVerticalShaderProgram->buildProgram();
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

	// first let's create some mipmaps of the original image
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture( GL_TEXTURE_2D, iOriginalTexture );
	//glGenerateMipmapEXT( GL_TEXTURE_2D );

	iOriginalFBO->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////////glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );
	iLogAvgLum.setValue( logAverage );
	iBrightpassShaderProgram->useProgram();
		RenderSceneOnQuad( iOriginalTexture );
	iBrightpassShaderProgram->disableProgram();

	//const int width = 4;
	//const int height = 4;
	//float result[width*height*4];
	//glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
	//glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, result);
	FrameBufferObject::unbind();

	//// first let's create some mipmaps of the brightpass image
	glBindTexture( GL_TEXTURE_2D, iBrightpassTexture );
	glGenerateMipmapEXT( GL_TEXTURE_2D );

	//generate blurred images using the mipmaps
	TMipMapLevel mipmapLevel = EMipMapLevel128;
	TTextureID intermediateTextureNumber = ETextureId128;

	// connect finalblur FBO
	for (int textureCounter = 0 ; textureCounter < KNumberOfBlurLevels; textureCounter++)
	{
		BlurMipmap( iBrightpassTexture, textureCounter, iMipmapSize[textureCounter], mipmapLevel+textureCounter );
	}
	FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//RenderSceneOnQuad(iOriginalTexture);
	//RenderSceneOnQuad(iBrightpassTexture);
	//RenderSceneOnQuad(iHorizBlurredTexture[0]);
	//RenderSceneOnQuad(iFinalBlurredTexture[0]);
	RenderBloomEffect(iOriginalTexture, iFinalBlurredTexture);

	return iBlendedTexture;
}

void BloomEffect::BlurMipmap(  GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aMipmapLevel  )
{
	//glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aCounter]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// store current information (which FBO is connected + Drawbuffer)
	// store viewport size
	GLint oldViewPort[4];
	glGetIntegerv( GL_VIEWPORT, oldViewPort );

	/************************************************************************/
	/* setup mipmap viewport size for horizontal blur                       */
	/* connect the horizontal texture and render the                        */
	/* horizontal blurred texture using the original image texture          */
	/************************************************************************/
	glViewport(0, 0, aMipmapSize, aMipmapSize);
	iIntermediateFBO[aCounter]->bind();
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iMipmapLevelUniform.setValue( aMipmapLevel );
	iMipmapSizeUniform.setValue( aMipmapSize );
	iHorizontalShaderProgram->useProgram();
		RenderSceneOnQuad( aTextureID, aMipmapSize );
	iHorizontalShaderProgram->disableProgram();

	//GLfloat pixels[8*8*4];
	//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	//glReadPixels(0,0,aWidth, aHeight, GL_RGBA, GL_FLOAT, pixels);

	/************************************************************************/
	/* resize the view port to the total size of the vertical blurred image */
	/* connect the vertical texture, clean it and                           */
	/* render the horizontal texture using the vertical shader              */
	/************************************************************************/
	//glViewport(0, 0, iImageBlurWidth, iImageBlurHeight);
	//glViewport(0, 0, aMipmapSize, aMipmapSize);
	glDrawBuffer( GL_COLOR_ATTACHMENT1_EXT );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iHorizTextureSizeUniform.setValue( aMipmapSize );
	iVerticalShaderProgram->useProgram();
		RenderSceneOnQuad(iHorizBlurredTexture[aCounter], aMipmapSize);
	iVerticalShaderProgram->disableProgram();

	/************************************************************************/
	/* connect specific fbo and render final texture to the drawbuffer      */
	/* upscale blurred image                                                */
	/************************************************************************/

	//glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aCounter]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//aUpscaleFBO->bind();
	//glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//RenderSceneOnQuad(iFinalBlurredTexture[aCounter]);
	//
	FrameBufferObject::unbind();
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

void BloomEffect::RenderSceneOnQuad( GLuint aTextureID, GLuint aVertexSize )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, aVertexSize, 0, aVertexSize );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, aTextureID );

	glBegin(GL_QUADS);
	
	glTexCoord2f( 0, 0 );
	glVertex2f(           0,           0 );

	glTexCoord2f( 1, 0 );
	glVertex2f( aVertexSize,           0 );
	
	glTexCoord2f( 1, 1 );
	glVertex2f( aVertexSize, aVertexSize );

	glTexCoord2f( 0, 1 );
	glVertex2f(           0, aVertexSize );
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
