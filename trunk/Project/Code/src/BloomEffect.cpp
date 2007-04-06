#include "BloomEffect.h"

BloomEffect::BloomEffect(unsigned int width, unsigned int height)
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
	delete iFinalBlurFBO;

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		delete iIntermediateFBO[i];
	}
}

void BloomEffect::Init()
{
	InitTextures();
	InitFramebufferObject();
	InitShaders();
}

void BloomEffect::InitTextures()
{
	iMipmapSize[0] = ETextureSize128;
	iMipmapSize[1] = ETextureSize64;
	iMipmapSize[2] = ETextureSize32;
	iMipmapSize[3] = ETextureSize16;

	iOriginalFBO = new FrameBufferObject();
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

	FrameBufferObject::unbind();


	// create a texture for the horizontal blur pass
	glGenTextures(KNumberOfBlurLevels, iHorizBlurredTexture);
	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurLevels, iFinalBlurredTexture);

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i] = new FrameBufferObject();
		iIntermediateFBO[i]->bind();

		glBindTexture(GL_TEXTURE_2D, iHorizBlurredTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		//iIntermediateFBO->attachDepthBuffer( depthBufferId );
		iIntermediateFBO[i]->attachTexture(iHorizBlurredTexture[i], GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);
		iIntermediateFBO[i]->attachTexture(iFinalBlurredTexture[i], GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);

		FrameBufferObject::unbind();
	}


	iFinalBlurFBO = new FrameBufferObject();

	iFinalBlurFBO->bind();
	iFinalBlurFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurLevels, iUpscaledTexture);

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);
		iFinalBlurFBO->attachTexture(iUpscaledTexture[i], GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0);
	}

	FrameBufferObject::unbind();
}

void BloomEffect::InitFramebufferObject()
{
}

/*
* Initializes the program shaders	
*/
void BloomEffect::InitShaders()
{
	iHorizontalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur.frag");
	iVerticalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur.frag");
	iBlenderFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/bloomBlender.frag");

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	iBlenderShaderProgram = new ShaderProgram();

	iHorizontalShaderProgram->attachShader( *iHorizontalBlurFragmentShader );
	iVerticalShaderProgram->attachShader( *iVerticalBlurFragmentShader );
	iBlenderShaderProgram->attachShader( *iBlenderFragmentShader );

	iTextureOriginalUniform.setValue( 0 );
	iTextureOriginalUniform.setName("originalTexture");

	iTextureHorizontalUniform.setValue( 0 );
	iTextureHorizontalUniform.setName("originalTexture");

	iMipmapSizeUniform.setValue( 32 );
	iMipmapSizeUniform.setName("mipmapSize");

	iMipmapLevelUniform.setValue( 4.0f );
	iMipmapLevelUniform.setName("mipmapLevel");

	iHorizTextureSizeUniform.setValue( 32 );
	iHorizTextureSizeUniform.setName("mipmapSize");

	iHorizBlurWeight1Uniform.setValue( 0.22508352f );
	iHorizBlurWeight1Uniform.setName("blurWeight1");
	iHorizBlurWeight2Uniform.setValue( 0.11098164f );
	iHorizBlurWeight2Uniform.setName("blurWeight2");
	iHorizBlurWeight3Uniform.setValue( 0.01330373f );
	iHorizBlurWeight3Uniform.setName("blurWeight3");

	iVertBlurWeight1Uniform.setValue( 0.22508352f );
	iVertBlurWeight1Uniform.setName("blurWeight1");
	iVertBlurWeight2Uniform.setValue( 0.11098164f );
	iVertBlurWeight2Uniform.setName("blurWeight2");
	iVertBlurWeight3Uniform.setValue( 0.01330373f );
	iVertBlurWeight3Uniform.setName("blurWeight3");



	//iHorizMipMapLevelUniform.setValue( 4.0f );
	//iHorizMipMapLevelUniform.setName("mipmapLevel");

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


	//iBlurDeltaUniform.setName("delta");
	//iBlurDeltaUniform.setValue( 0.01f );

	iHorizontalShaderProgram->addUniformObject( &iTextureOriginalUniform );
	iHorizontalShaderProgram->addUniformObject( &iMipmapSizeUniform );
	iHorizontalShaderProgram->addUniformObject( &iMipmapLevelUniform );
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight1Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight2Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight3Uniform);
	//iHorizontalShaderProgram->addUniformObject( &iBlurDeltaUniform );

	iVerticalShaderProgram->addUniformObject( &iTextureHorizontalUniform );
	iVerticalShaderProgram->addUniformObject( &iHorizTextureSizeUniform );
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight1Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight2Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight3Uniform);

	//iVerticalShaderProgram->addUniformObject( &iHorizMipMapLevelUniform );
	//iVerticalShaderProgram->addUniformObject( &iBlurDeltaUniform );

	//iBlenderShaderProgram->addUniformObject( &iTextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderOriginalTextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur1TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur2TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur3TextureUniform );
	iBlenderShaderProgram->addUniformObject( &iBlenderBlur4TextureUniform );

	iHorizontalShaderProgram->buildProgram();
	iVerticalShaderProgram->buildProgram();
	iBlenderShaderProgram->buildProgram();
}

void BloomEffect::Begin()
{
	// ******* RENDER TO THE FRAMEBUFFER ********
	//bind the FBO, and the associated texture.
	iOriginalFBO->bind();

	glGetIntegerv( GL_VIEWPORT, iOldViewPort );	

	glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );
	//initialize original texture
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

	// #### FIRST STEP: Draw the object into the original texture: 'originalTexture'
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/************************************************************************/
	/* ######################### DRAW AFTER THIS !!!                        */
	/************************************************************************/
}



void BloomEffect::End()
{
	/************************************************************************/
	/* ######################### DRAW WAS DONE BEFORE THIS !!!              */
	/************************************************************************/

	FrameBufferObject::unbind();
	// first let's create some mipmaps of the original image
	glBindTexture( GL_TEXTURE_2D, iOriginalTexture );
	glGenerateMipmapEXT( GL_TEXTURE_2D );

	//generate blurred images using the mipmaps
	TMipMapLevel mipmapLevel = EMipMapLevel128;
	TTextureID intermediateTextureNumber = ETextureId128;

	// connect finalblur FBO
	for (int i = 0 ; i < KNumberOfBlurLevels; i++)
	{
		iFinalBlurFBO->bind();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + i);
		glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );

		BlurMipmap( iOriginalTexture, intermediateTextureNumber+i, iMipmapSize[i], mipmapLevel+i, iFinalBlurFBO );
	}
	FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//RenderSceneOnQuad(iHorizBlurredTexture[3]);
	//RenderSceneOnQuad(iFinalBlurredTexture[3]);
	//RenderSceneOnQuad(iUpscaledTexture[2]);
	RenderBloomEffect(iOriginalTexture, iUpscaledTexture);
}

void BloomEffect::BlurMipmap( GLuint aTextureID, GLuint aIntermediateTextureNumber, GLuint aMipmapSize, GLuint aMipmapLevel, FrameBufferObject* aFinalFBO )
{
	glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aIntermediateTextureNumber]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


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
	iIntermediateFBO[aIntermediateTextureNumber]->bind();
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iMipmapLevelUniform.setValue( aMipmapLevel );
	iMipmapSizeUniform.setValue( aMipmapSize );
	iHorizontalShaderProgram->useProgram();
	RenderSceneOnQuad( aTextureID );
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
		//UpscaleTexture(iHorizBlurredTexture, aUpscaleFactor);
		RenderSceneOnQuad( iHorizBlurredTexture[aIntermediateTextureNumber] );
	iVerticalShaderProgram->disableProgram();

	/************************************************************************/
	/* connect specific fbo and render final texture to the drawbuffer      */
	/* upscale blurred image                                                */
	/************************************************************************/

	glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aIntermediateTextureNumber]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	aFinalFBO->bind();
	glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSceneOnQuad(iFinalBlurredTexture[aIntermediateTextureNumber]);
	
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

void BloomEffect::UpscaleTexture( GLuint aTextureID, GLfloat aTextureCoord )
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

	glTexCoord2f( aTextureCoord, 0 );
	glVertex2f( 1, -1 );

	glTexCoord2f( aTextureCoord, aTextureCoord );
	glVertex2f( 1, 1 );

	glTexCoord2f( 0, aTextureCoord );
	glVertex2f( -1, 1 );
	glEnd();


	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}

void BloomEffect::RenderBloomEffect( GLuint aOriginalTexture, GLuint* aUpscaledTexture )
{
	iBlenderShaderProgram->useProgram();
		RenderSceneOnQuad(aOriginalTexture, aUpscaledTexture );
	iBlenderShaderProgram->disableProgram();

	//RenderSceneOnQuad( iFinalBlurredTexture );
	//RenderSceneOnQuad( iUpscaledTexture[0] );
	//RenderSceneOnQuad( iUpscaledTexture[1] );
	//RenderSceneOnQuad( iUpscaledTexture[2] );
	//RenderSceneOnQuad( iUpscaledTexture[3] );
}

void BloomEffect::RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aUpscaledTexture )
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
		glBindTexture( GL_TEXTURE_2D, aUpscaledTexture[i] );
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
