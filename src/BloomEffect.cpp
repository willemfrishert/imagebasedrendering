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
	delete iUpscaleBlurFBO;

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		delete iIntermediateFBO[i];
	}
}

void BloomEffect::Init()
{
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

	iUpscaleBlurFBO->bind();
	iUpscaleBlurFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurLevels, iUpscaledTexture);

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);
		iUpscaleBlurFBO->attachTexture(iUpscaledTexture[i], GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0);
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
	
	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i] = new FrameBufferObject();
	}

	iUpscaleBlurFBO = new FrameBufferObject();

	iBlendedFBO = new FrameBufferObject();
}

/*
* Initializes the program shaders	
*/
void BloomEffect::InitShaders()
{
	InitCodecShaderObject();

	InitBlurShaders();
	InitBlendShader();
}

void BloomEffect::InitCodecShaderObject()
{
	iCodecRGBEFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/codecRGBE.frag");
}

void BloomEffect::InitBlurShaders()
{
	iHorizontalBlurFragmentShader[0] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur128.frag");
	iVerticalBlurFragmentShader[0] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur128.frag");

	//iHorizontalBlurFragmentShader[1] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur64.frag");
	//iVerticalBlurFragmentShader[1] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur64.frag");

	//iHorizontalBlurFragmentShader[2] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur32.frag");
	//iVerticalBlurFragmentShader[2] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur32.frag");

	//iHorizontalBlurFragmentShader[3] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur16.frag");
	//iVerticalBlurFragmentShader[3] = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur16.frag");
	
	//generate blurred images using the mipmaps
	TMipMapLevel mipmapLevel = EMipMapLevel128;
	TTextureID intermediateTextureNumber = ETextureId128;

	for (int i = 0; i < KNumberOfBlurShaders; i++)
	{
		iHorizontalShaderProgram[i] = new ShaderProgram();
		iVerticalShaderProgram[i] = new ShaderProgram();
		iHorizontalShaderProgram[i]->attachShader( *iCodecRGBEFragmentShader );
		iHorizontalShaderProgram[i]->attachShader( *(iHorizontalBlurFragmentShader[i]) );
		iVerticalShaderProgram[i]->attachShader( *iCodecRGBEFragmentShader );
		iVerticalShaderProgram[i]->attachShader( *(iVerticalBlurFragmentShader[i]) );

		iTextureOriginalUniform[i].setValue( 0 );
		iTextureOriginalUniform[i].setName("originalTexture");

		iTextureHorizontalUniform[i].setValue( 0 );
		iTextureHorizontalUniform[i].setName("originalTexture");

		iMipmapSizeUniform[i].setValue( iMipmapSize[i] );
		iMipmapSizeUniform[i].setName("mipmapSize");

		iMipmapLevelUniform[i].setValue( mipmapLevel+i );
		iMipmapLevelUniform[i].setName("mipmapLevel");

		iHorizTextureSizeUniform[i].setValue( iMipmapSize[i] );
		iHorizTextureSizeUniform[i].setName("mipmapSize");

		iHorizontalShaderProgram[i]->addUniformObject( &iTextureOriginalUniform[i] );
		iHorizontalShaderProgram[i]->addUniformObject( &iMipmapSizeUniform[i] );
		iHorizontalShaderProgram[i]->addUniformObject( &iMipmapLevelUniform[i] );
		iVerticalShaderProgram[i]->addUniformObject( &iTextureHorizontalUniform[i] );
		iVerticalShaderProgram[i]->addUniformObject( &iHorizTextureSizeUniform[i] );

		iHorizBlurWeight1Uniform[i].setName("blurWeight1");
		iHorizBlurWeight2Uniform[i].setName("blurWeight2");
		iHorizBlurWeight3Uniform[i].setName("blurWeight3");
		iHorizBlurWeight4Uniform[i].setName("blurWeight4");
		iHorizBlurWeight5Uniform[i].setName("blurWeight5");

		iVertBlurWeight1Uniform[i].setName("blurWeight1");
		iVertBlurWeight2Uniform[i].setName("blurWeight2");
		iVertBlurWeight3Uniform[i].setName("blurWeight3");
		iVertBlurWeight4Uniform[i].setName("blurWeight4");
		iVertBlurWeight5Uniform[i].setName("blurWeight5");
	}

	iHorizBlurWeight1Uniform[0].setValue( 0.375f );
	iHorizBlurWeight2Uniform[0].setValue( 0.250f );
	iHorizBlurWeight3Uniform[0].setValue( 0.062f );

	iVertBlurWeight1Uniform[0].setValue( 0.375f );
	iVertBlurWeight2Uniform[0].setValue( 0.250f );
	iVertBlurWeight3Uniform[0].setValue( 0.062f );

	//iHorizBlurWeight1Uniform[0].setValue( 1.0f );
	//iHorizBlurWeight2Uniform[0].setValue( 0.0f );
	//iHorizBlurWeight3Uniform[0].setValue( 0.0f );

	//iVertBlurWeight1Uniform[0].setValue( 1.0f );
	//iVertBlurWeight2Uniform[0].setValue( 0.0f );
	//iVertBlurWeight3Uniform[0].setValue( 0.0f );

	iHorizontalShaderProgram[0]->addUniformObject( &iHorizBlurWeight1Uniform[0]);
	iHorizontalShaderProgram[0]->addUniformObject( &iHorizBlurWeight2Uniform[0]);
	iHorizontalShaderProgram[0]->addUniformObject( &iHorizBlurWeight3Uniform[0]);

	iVerticalShaderProgram[0]->addUniformObject( &iVertBlurWeight1Uniform[0]);
	iVerticalShaderProgram[0]->addUniformObject( &iVertBlurWeight2Uniform[0]);
	iVerticalShaderProgram[0]->addUniformObject( &iVertBlurWeight3Uniform[0]);


	//iHorizBlurWeight1Uniform[1].setValue( 0.11792800956558f );
	//iHorizBlurWeight2Uniform[1].setValue( 0.08333362487070f );
	//iHorizBlurWeight3Uniform[1].setValue( 0.02940560966189f );

	//iVertBlurWeight1Uniform[1].setValue( 0.11792800956558f );
	//iVertBlurWeight2Uniform[1].setValue( 0.08333362487070f );
	//iVertBlurWeight3Uniform[1].setValue( 0.02940560966189f );

	//iHorizontalShaderProgram[1]->addUniformObject( &iHorizBlurWeight1Uniform[1]);
	//iHorizontalShaderProgram[1]->addUniformObject( &iHorizBlurWeight2Uniform[1]);
	//iHorizontalShaderProgram[1]->addUniformObject( &iHorizBlurWeight3Uniform[1]);

	//iVerticalShaderProgram[1]->addUniformObject( &iVertBlurWeight1Uniform[1]);
	//iVerticalShaderProgram[1]->addUniformObject( &iVertBlurWeight2Uniform[1]);
	//iVerticalShaderProgram[1]->addUniformObject( &iVertBlurWeight3Uniform[1]);


	//iHorizBlurWeight1Uniform[2].setValue( 0.05429740718587f );
	//iHorizBlurWeight2Uniform[2].setValue( 0.04653270917128f );
	//iHorizBlurWeight3Uniform[2].setValue( 0.02928842905960f );
	//iHorizBlurWeight4Uniform[2].setValue( 0.01353917841268f );

	//iVertBlurWeight1Uniform[2].setValue( 0.05429740718587f );
	//iVertBlurWeight2Uniform[2].setValue( 0.04653270917128f );
	//iVertBlurWeight3Uniform[2].setValue( 0.02928842905960f );
	//iVertBlurWeight4Uniform[2].setValue( 0.01353917841268f );

	//iHorizontalShaderProgram[2]->addUniformObject( &iHorizBlurWeight1Uniform[2]);
	//iHorizontalShaderProgram[2]->addUniformObject( &iHorizBlurWeight2Uniform[2]);
	//iHorizontalShaderProgram[2]->addUniformObject( &iHorizBlurWeight3Uniform[2]);
	//iHorizontalShaderProgram[2]->addUniformObject( &iHorizBlurWeight4Uniform[2]);

	//iVerticalShaderProgram[2]->addUniformObject( &iVertBlurWeight1Uniform[2]);
	//iVerticalShaderProgram[2]->addUniformObject( &iVertBlurWeight2Uniform[2]);
	//iVerticalShaderProgram[2]->addUniformObject( &iVertBlurWeight3Uniform[2]);
	//iVerticalShaderProgram[2]->addUniformObject( &iVertBlurWeight4Uniform[2]);


	//iHorizBlurWeight1Uniform[3].setValue( 0.02346114926271f );
	//iHorizBlurWeight2Uniform[3].setValue( 0.02219329629650f );
	//iHorizBlurWeight3Uniform[3].setValue( 0.01878621973007f );
	//iHorizBlurWeight4Uniform[3].setValue( 0.01422990633993f );
	//iHorizBlurWeight5Uniform[3].setValue( 0.00964516681132f );

	//iVertBlurWeight1Uniform[3].setValue( 0.02346114926271f );
	//iVertBlurWeight2Uniform[3].setValue( 0.02219329629650f );
	//iVertBlurWeight3Uniform[3].setValue( 0.01878621973007f );
	//iVertBlurWeight4Uniform[3].setValue( 0.01422990633993f );
	//iVertBlurWeight5Uniform[3].setValue( 0.00964516681132f );

	//iHorizontalShaderProgram[3]->addUniformObject( &iHorizBlurWeight1Uniform[3]);
	//iHorizontalShaderProgram[3]->addUniformObject( &iHorizBlurWeight2Uniform[3]);
	//iHorizontalShaderProgram[3]->addUniformObject( &iHorizBlurWeight3Uniform[3]);
	//iHorizontalShaderProgram[3]->addUniformObject( &iHorizBlurWeight4Uniform[3]);
	//iHorizontalShaderProgram[3]->addUniformObject( &iHorizBlurWeight5Uniform[3]);

	//iVerticalShaderProgram[3]->addUniformObject( &iVertBlurWeight1Uniform[3]);
	//iVerticalShaderProgram[3]->addUniformObject( &iVertBlurWeight2Uniform[3]);
	//iVerticalShaderProgram[3]->addUniformObject( &iVertBlurWeight3Uniform[3]);
	//iVerticalShaderProgram[3]->addUniformObject( &iVertBlurWeight4Uniform[3]);
	//iVerticalShaderProgram[3]->addUniformObject( &iVertBlurWeight5Uniform[3]);

	for (int i = 0; i < KNumberOfBlurShaders; i++)
	{
		iHorizontalShaderProgram[i]->buildProgram();
		iVerticalShaderProgram[i]->buildProgram();
	}
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



GLuint BloomEffect::End()
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
	for (int textureCounter = 0 ; textureCounter < KNumberOfBlurLevels; textureCounter++)
	{
		iUpscaleBlurFBO->bind();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + textureCounter);
		glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );

		BlurMipmap( iOriginalTexture, textureCounter, iMipmapSize[textureCounter], mipmapLevel+textureCounter, iUpscaleBlurFBO );
	}
	FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//RenderSceneOnQuad(iOriginalTexture);
	//RenderSceneOnQuad(iHorizBlurredTexture[0]);
	//RenderSceneOnQuad(iFinalBlurredTexture[0]);
	//RenderSceneOnQuad(iUpscaledTexture[0]);
	RenderBloomEffect(iOriginalTexture, iUpscaledTexture);

	return iBlendedTexture;
}

void BloomEffect::BlurMipmap( GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aMipmapLevel, FrameBufferObject* aUpscaleFBO )
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
	iMipmapLevelUniform[0].setValue( aMipmapLevel );
	iMipmapSizeUniform[0].setValue( aMipmapSize );
	iHorizontalShaderProgram[0]->useProgram();
	RenderSceneOnQuad( aTextureID );
	iHorizontalShaderProgram[0]->disableProgram();

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
	iHorizTextureSizeUniform[0].setValue( aMipmapSize );
	iVerticalShaderProgram[0]->useProgram();
		//UpscaleTexture(iHorizBlurredTexture, aUpscaleFactor);
		RenderSceneOnQuad( iHorizBlurredTexture[aCounter] );
	iVerticalShaderProgram[0]->disableProgram();

	/************************************************************************/
	/* connect specific fbo and render final texture to the drawbuffer      */
	/* upscale blurred image                                                */
	/************************************************************************/

	//glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aCounter]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	aUpscaleFBO->bind();
	glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSceneOnQuad(iFinalBlurredTexture[aCounter]);
	
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
//	iBlendedFBO->bind();
//	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iBlenderShaderProgram->useProgram();
		RenderSceneOnQuad(aOriginalTexture, aUpscaledTexture );
	iBlenderShaderProgram->disableProgram();
//	FrameBufferObject::unbind();

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
