#include "BlurTexture.h"
#include "Utility.h"

BlurTexture::BlurTexture()
: iOriginalImageSize( ETextureSize512 )
{
	Init();
}

BlurTexture::~BlurTexture()
{
	delete iOriginalFBO;

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		delete iIntermediateFBO[i];
	}
	glDeleteTextures(1, &iOriginalTexture);
	glDeleteTextures(4, iHorizBlurredTexture);
	glDeleteTextures(4, iFinalBlurredTexture);
}

void BlurTexture::Init()
{
	InitFramebufferObject();
	InitTextures();
	InitShaders();
}

void BlurTexture::InitTextures()
{
	//iMipmapSize[0] = ETextureSize256;
	//iMipmapSize[1] = ETextureSize128;
	//iMipmapSize[2] = ETextureSize64;
	//iMipmapSize[3] = ETextureSize32;

	//iMipmapSize[0] = ETextureSize128;
	//iMipmapSize[1] = ETextureSize64;
	//iMipmapSize[2] = ETextureSize32;
	//iMipmapSize[3] = ETextureSize16;

	iMipmapSize[0] = ETextureSize128;
	iMipmapSize[1] = ETextureSize32;
	iMipmapSize[2] = ETextureSize16;
	iMipmapSize[3] = ETextureSize8;

	iOriginalFBO->bind();

	iOriginalFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexture);

	glBindTexture(GL_TEXTURE_2D, iOriginalTexture);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	iOriginalFBO->attachTexture(iOriginalTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	// create a texture for the horizontal blur pass
	glGenTextures(KNumberOfBlurLevels, iHorizBlurredTexture);
	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurLevels, iFinalBlurredTexture);
	// create a texture for downscaling
	glGenTextures(KNumberOfBlurLevels, iDownScaledTexture);
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

		glBindTexture(GL_TEXTURE_2D, iDownScaledTexture[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		//iIntermediateFBO->attachDepthBuffer( depthBufferId );
		iIntermediateFBO[i]->attachTexture(iHorizBlurredTexture[i], GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);
		iIntermediateFBO[i]->attachTexture(iFinalBlurredTexture[i], GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);
		iIntermediateFBO[i]->attachTexture(iDownScaledTexture[i],   GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, 0);

		FrameBufferObject::unbind();
	}
}

void BlurTexture::InitFramebufferObject()
{
	iOriginalFBO = new FrameBufferObject();	

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i] = new FrameBufferObject();
	}
}

/*
* Initializes the program shaders	
*/
void BlurTexture::InitShaders()
{
	InitCodecShaderObject();
	InitBlurShaders();
}

void BlurTexture::InitCodecShaderObject()
{
	iCodecRGBEFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/codecRGBE.frag");
}

void BlurTexture::InitBlurShaders()
{
	iHorizontalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur.frag");

	iVerticalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur.frag");

	TMipMapLevel mipmapLevel = EMipMapLevel128;
	TTextureID intermediateTextureNumber = ETextureId128;

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	iHorizontalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iHorizontalShaderProgram->attachShader( *(iHorizontalBlurFragmentShader) );

	iVerticalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iVerticalShaderProgram->attachShader( *(iVerticalBlurFragmentShader) );

	iTextureOriginalUniform.setValue( 0 );
	iTextureOriginalUniform.setName("originalTexture");

	iTextureHorizontalUniform.setValue( 0 );
	iTextureHorizontalUniform.setName("originalTexture");

	iMipmapSizeUniform.setValue( iMipmapSize[0] );
	iMipmapSizeUniform.setName("mipmapSize");

	//iMipmapLevelUniform.setValue( mipmapLevel );
	//iMipmapLevelUniform.setName("mipmapLevel");

	iHorizTextureSizeUniform.setValue( iMipmapSize[0] );
	iHorizTextureSizeUniform.setName("mipmapSize");

	iHorizontalShaderProgram->addUniformObject( &iTextureOriginalUniform );
	iHorizontalShaderProgram->addUniformObject( &iMipmapSizeUniform );
	//iHorizontalShaderProgram->addUniformObject( &iMipmapLevelUniform );
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

GLuint* BlurTexture::processData(GLuint aTextureId)
{
	// ******* RENDER TO THE FRAMEBUFFER ********
	//bind the FBO, and the associated texture.
	iOriginalFBO->bind();
	//draw to original texture
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glGetIntegerv( GL_VIEWPORT, iOldViewPort );	

	glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	RenderSceneOnQuad(aTextureId );

	FrameBufferObject::unbind();

	glBindTexture( GL_TEXTURE_2D, iOriginalTexture );
	glGenerateMipmapEXT( GL_TEXTURE_2D );
	
	static GLuint blurpasses[] = {1, 5, 10, 15};

	// connect finalblur FBO
	for (int textureCounter = 0 ; textureCounter < KNumberOfBlurLevels; textureCounter++)
	{
		BlurMipmap( iOriginalTexture, textureCounter, iMipmapSize[textureCounter], blurpasses[textureCounter] );
	}
	//FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	return iFinalBlurredTexture;
}

void BlurTexture::BlurMipmap(  GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aBlurPasses  )
{
	//glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture[aCounter]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



	/************************************************************************/
	/* setup mipmap viewport size                                           */
	/************************************************************************/
	GLint oldViewPort[4];
	glGetIntegerv( GL_VIEWPORT, oldViewPort );
	glViewport(0, 0, aMipmapSize, aMipmapSize);
	iIntermediateFBO[aCounter]->bind();

	for (int i=0; i < aBlurPasses; i++)
	{
		/*********************************************************************************/
		/* connect the horizontal texture and render the                                 */
		/* horizontal blurred texture using the original image texture or vertical image */
		/*********************************************************************************/

		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		glClearColor(0.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//iMipmapLevelUniform.setValue( 0 );
		iMipmapSizeUniform.setValue( aMipmapSize );
		iHorizontalShaderProgram->useProgram();
		if (i == 0)
		{
			RenderSceneOnQuad( aTextureID, aMipmapSize );
		}
		else
		{
			RenderSceneOnQuad( iFinalBlurredTexture[aCounter], aMipmapSize );
		}
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
		//RenderSceneOnQuad(iHorizBlurredTexture[aCounter], aMipmapSize);
		RenderSceneOnQuad(iHorizBlurredTexture[aCounter], aMipmapSize);
		iVerticalShaderProgram->disableProgram();
	}

	FrameBufferObject::unbind();
}

void BlurTexture::RenderSceneOnQuad( GLuint aTextureID )
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

void BlurTexture::RenderSceneOnQuad( GLuint aTextureID, GLuint aVertexSize )
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

