#include "BloomEffect.h"

BloomEffect::BloomEffect(unsigned int width, unsigned int height)
: iImageWinWidth( width )
, iImageWinHeight( height )
, iImageBlurWidth( 128 )
, iImageBlurHeight( 128 )
{
	Init();
}

BloomEffect::~BloomEffect()
{
		delete iOriginalFBO;
		delete iFinalBlurFBO;
		delete iIntermediateFBO;
}

void BloomEffect::Init()
{
	InitTextures();
	InitFramebufferObject();
	InitShaders();
}

void BloomEffect::InitTextures()
{
	iOriginalFBO = new FrameBufferObject();
	iOriginalFBO->bind();

	iOriginalFBO->attachDepthRenderBuffer(iImageWinWidth, iImageWinHeight);

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexture);

	glBindTexture(GL_TEXTURE_2D, iOriginalTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	iOriginalFBO->attachTexture(iOriginalTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	FrameBufferObject::unbind();


	iIntermediateFBO = new FrameBufferObject();

	iIntermediateFBO->bind();

	

	//iIntermediateFBO->attachDepthRenderBuffer(iImageBlurWidth, iImageBlurHeight);

	//GLuint depthBufferId;
	//glGenTextures(1, &depthBufferId);

	//// initialize depth buffer
	//glBindTexture(GL_TEXTURE_2D, depthBufferId);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, iImageBlurWidth, iImageBlurHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	// create a texture for the horizontal blur pass
	glGenTextures(1, &iHorizBlurredTexture);
	// create a texture for the vertical blur and thus final pass
	//glGenTextures(1, &iFinalBlurredTexture);

	glBindTexture(GL_TEXTURE_2D, iHorizBlurredTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	//iIntermediateFBO->attachDepthBuffer( depthBufferId );
	iIntermediateFBO->attachTexture(iHorizBlurredTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);
	iIntermediateFBO->attachTexture(iFinalBlurredTexture, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);

	//glBindTexture(GL_TEXTURE_RECTANGLE_ARB, iHorizBlurredTexture);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	//glBindTexture(GL_TEXTURE_RECTANGLE_ARB, iFinalBlurredTexture);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	//iIntermediateFBO->attachDepthBuffer( depthBufferId );
	//iIntermediateFBO->attachTexture(iHorizBlurredTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, 0);
	//iIntermediateFBO->attachTexture(iFinalBlurredTexture, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, 0);

	FrameBufferObject::unbind();

	iFinalBlurFBO = new FrameBufferObject();

	iFinalBlurFBO->bind();
	iFinalBlurFBO->attachDepthRenderBuffer(iImageWinHeight, iImageWinHeight);

	// create a texture for the vertical blur and thus final pass
	glGenTextures(KNumberOfBlurMaps, iUpscaledTexture);

	glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[2]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[3]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	iFinalBlurFBO->attachTexture(iUpscaledTexture[0], GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);
	iFinalBlurFBO->attachTexture(iUpscaledTexture[1], GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);
	iFinalBlurFBO->attachTexture(iUpscaledTexture[2], GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, 0);
	iFinalBlurFBO->attachTexture(iUpscaledTexture[3], GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D, 0);

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
	//iHorizontalBlurVertexShader = new ShaderObject(GL_VERTEX_SHADER, "./shader/horizBlur.vert");
	iHorizontalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/horizBlur.frag");

	//iVerticalBlurVertexShader = new ShaderObject(GL_VERTEX_SHADER, "./shader/vertBlur.vert");
	iVerticalBlurFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/vertBlur.frag");

	//iBlenderVertexShader = new ShaderObject(GL_VERTEX_SHADER, "./shader/glowblender.vert");
	iBlenderFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/bloomBlender.frag");

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	iBlenderShaderProgram = new ShaderProgram();

	//iHorizontalShaderProgram->attachShader( *iHorizontalBlurVertexShader );
	iHorizontalShaderProgram->attachShader( *iHorizontalBlurFragmentShader );

	//iVerticalShaderProgram->attachShader( *iVerticalBlurVertexShader );
	iVerticalShaderProgram->attachShader( *iVerticalBlurFragmentShader );

	//iBlenderShaderProgram->attachShader( *iBlenderVertexShader );
	iBlenderShaderProgram->attachShader( *iBlenderFragmentShader );

	iTextureOriginalUniform.setValue( 0 );
	iTextureOriginalUniform.setName("originalTexture");

	iTextureHorizontalUniform.setValue( 0 );
	iTextureHorizontalUniform.setName("originalTexture");

	iMipmapSizeUniform.setValue( 32 );
	iMipmapSizeUniform.setName("mipmapSize");

	iMipmapLevelUniform.setName("mipmapLevel");
	iMipmapLevelUniform.setValue( 4.0f );

	iHorizTextureSizeUniform.setValue( iImageBlurWidth );
	iHorizTextureSizeUniform.setName("mipmapSize");

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
	//iHorizontalShaderProgram->addUniformObject( &iBlurDeltaUniform );

	iVerticalShaderProgram->addUniformObject( &iTextureHorizontalUniform );
	iVerticalShaderProgram->addUniformObject( &iHorizTextureSizeUniform );
	//iVerticalShaderProgram->addUniformObject( &iTextureUniform );
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

	glViewport( 0, 0, iImageWinWidth, iImageWinHeight );
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
	//GLint mipmapSize = 32;
	//GLfloat mipmapLevel = 4.0f;
	//GLfloat upscale = 0.25f;
	GLint mipmapSize = 128;
	GLfloat mipmapLevel = iImageBlurWidth/mipmapSize;
	GLfloat upscale = 1.0f/mipmapLevel;

	// connect finalblur FBO
	for (int blurCounter = 0 ; blurCounter < KNumberOfBlurMaps; blurCounter++)
	{
		iFinalBlurFBO->bind();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + blurCounter);
		glViewport( 0, 0, iImageWinWidth, iImageWinHeight );

		BlurMipmap( iOriginalTexture, mipmapSize, mipmapLevel, upscale, iFinalBlurFBO );

		mipmapSize *= 0.5f;
		mipmapLevel += 1.0f;
		upscale *= 0.5f;
	}
	FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	RenderBloomEffect(iOriginalTexture, iUpscaledTexture);
}

void BloomEffect::BlurMipmap( GLuint aTextureID,  GLuint aMipmapSize, GLfloat aMipmapLevel, GLfloat aUpscaleFactor, FrameBufferObject* aFinalFBO )
{
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
	iIntermediateFBO->bind();
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iMipmapLevelUniform.setValue( aMipmapLevel );
	iMipmapSizeUniform.setValue(aMipmapSize);
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
	glViewport(0, 0, iImageBlurWidth, iImageBlurHeight);
	glDrawBuffer( GL_COLOR_ATTACHMENT1_EXT );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iVerticalShaderProgram->useProgram();
		RenderSceneOnQuad( iHorizBlurredTexture );
	iVerticalShaderProgram->disableProgram();

	/************************************************************************/
	/* connect specific fbo and render final texture to the drawbuffer      */
	/* upscale blurred image                                                */
	/************************************************************************/
	aFinalFBO->bind();
	glViewport( oldViewPort[0], oldViewPort[1], oldViewPort[2], oldViewPort[3] );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UpscaleTexture(iFinalBlurredTexture, aUpscaleFactor);
	
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
	for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps; blurCounter++)
	{
		glActiveTexture( GL_TEXTURE1 + blurCounter );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, aUpscaledTexture[blurCounter] );
	}

	//RENDER MULTITEXTURE ON QUAD
	glBegin(GL_QUADS);
	
		for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps + 1; blurCounter++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + blurCounter, 0, 0 );
		}
		glVertex2f( -1.0f, -1.0f );

		for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps + 1; blurCounter++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + blurCounter, 1, 0 );
		}
		glVertex2f( 1.0f, -1.0f );

		for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps + 1; blurCounter++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + blurCounter, 1, 1 );
		}
		glVertex2f( 1.0f, 1.0f );

		for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps + 1; blurCounter++)
		{
			glMultiTexCoord2d( GL_TEXTURE0 + blurCounter, 0, 1 );
		}
		glVertex2f( -1.0f, 1.0f );
	glEnd();

	// CLEANING MULTITEXTURING
	for (int blurCounter = 0; blurCounter < KNumberOfBlurMaps; blurCounter++)
	{
		glActiveTexture( GL_TEXTURE1 + blurCounter );
		glDisable( GL_TEXTURE_2D );
	}

	glActiveTexture( GL_TEXTURE0 );
	glDisable( GL_TEXTURE_2D );

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}
