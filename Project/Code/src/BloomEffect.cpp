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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, iFinalBlurredTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iImageBlurWidth, iImageBlurHeight, 0, GL_RGBA, GL_FLOAT, NULL);

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

	//iFinalBlurFBO = new FrameBufferObject();

	//iFinalBlurFBO->bind();
	//iFinalBlurFBO->attachDepthRenderBuffer(iImageBlurWidth, iImageBlurHeight);

	//// create a texture for the vertical blur and thus final pass
	//glGenTextures(KNumberOfBlurMaps, iUpscaledTexture);

	//glBindTexture(GL_TEXTURE_2D, iUpscaledTexture[0]);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iImageWinWidth, iImageWinHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	//iFinalBlurFBO->attachTexture(iUpscaledTexture[0], GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	//FrameBufferObject::unbind();
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
	//iBlenderFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/glowblender.frag");

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	//iBlenderShaderProgram = new ShaderProgram();

	//iHorizontalShaderProgram->attachShader( *iHorizontalBlurVertexShader );
	iHorizontalShaderProgram->attachShader( *iHorizontalBlurFragmentShader );

	//iVerticalShaderProgram->attachShader( *iVerticalBlurVertexShader );
	iVerticalShaderProgram->attachShader( *iVerticalBlurFragmentShader );

	//iBlenderShaderProgram->attachShader( *iBlenderVertexShader );
	//iBlenderShaderProgram->attachShader( *iBlenderFragmentShader );

	iTextureOriginalUniform.setValue( 0 );
	iTextureOriginalUniform.setName("originalTexture");

	iTextureHorizontalUniform.setValue( 0 );
	iTextureHorizontalUniform.setName("originalTexture");

	iMipmapSizeUniform.setValue( 32 );
	iMipmapSizeUniform.setName("mipmapSize");

	iHorizTextureSizeUniform.setValue( 128 );
	iHorizTextureSizeUniform.setName("mipmapSize");

	//iBlenderTextureUniform.setValue( 1 );
	//iBlenderTextureUniform.setName("originalTex");

	iMipmapLevelUniform.setName("mipmapLevel");
	iMipmapLevelUniform.setValue( 4.0f );

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
	//iBlenderShaderProgram->addUniformObject( &iBlenderTextureUniform );

	iHorizontalShaderProgram->buildProgram();
	iVerticalShaderProgram->buildProgram();
	//iBlenderShaderProgram->buildProgram();
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
    // viewportSize = textureSize = 32;
	// mipmap level = 4;
	// upscale = 0.25
	GLint mipmapSize = 32;
	GLfloat mipmapLevel = 4.0f;
	GLfloat upscale = 0.25;

	BlurMipmap( iOriginalTexture, mipmapSize, GL_COLOR_ATTACHMENT0_EXT, iImageWinWidth, iImageWinHeight );

	//glViewport( 0, 0, 32, 32 );

	//iIntermediateFBO->bind();
	////connect the horizontal texture
	//glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//iHorizontalShaderProgram->useProgram();
	//	RenderSceneOnQuad( iOriginalTexture );
	//iHorizontalShaderProgram->disableProgram();

	FrameBufferObject::unbind();

	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//UpscaleTexture(iOriginalTexture, GL_TEXTURE_2D, 128, 128);
	//RenderSceneOnQuad( iUpscaledTexture[0] );
	//UpscaleTexture( iHorizBlurredTexture, 0.25 );
	//UpscaleTexture(iFinalBlurredTexture, 0.25);
	//RenderSceneOnQuad( iHorizBlurredTexture, GL_TEXTURE_RECTANGLE_ARB, iOldViewPort[2], iOldViewPort[3] );
	//RenderSceneOnQuad( iFinalBlurredTexture, GL_TEXTURE_RECTANGLE_ARB, iOldViewPort[2], iOldViewPort[3] );
	//RenderSceneOnQuad( iHorizBlurredTexture, 128,128 );

	UpscaleTexture(iFinalBlurredTexture, 0.25);
	//RenderSceneOnQuad( iFinalBlurredTexture );
}

void BloomEffect::BlurMipmap(GLuint aTextureID,
							 GLuint aMipmapSize,
							 GLenum aDestinationMode,
							 GLuint aDestinationWidth,
							 GLuint aDestinationHeight)
{
	glViewport(0, 0, aMipmapSize, aMipmapSize);

	iIntermediateFBO->bind();
	//connect the horizontal texture
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iHorizontalShaderProgram->useProgram();
	RenderSceneOnQuad( aTextureID );
	iHorizontalShaderProgram->disableProgram();

	//GLfloat pixels[8*8*4];
	//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	//glReadPixels(0,0,aWidth, aHeight, GL_RGBA, GL_FLOAT, pixels);

	// resize the view port to the total size of the horizontal blurred image
	glViewport(0, 0, iImageBlurWidth, iImageBlurHeight);

	//connect the vertical texture
	glDrawBuffer( GL_COLOR_ATTACHMENT1_EXT );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iVerticalShaderProgram->useProgram();
		UpscaleTexture(iHorizBlurredTexture, 0.25);
//		RenderSceneOnQuad( iHorizBlurredTexture );
	iVerticalShaderProgram->disableProgram();

	//// upscale blurred image
	//iFinalBlurFBO->bind();

	//glDrawBuffer( aDestinationMode );

	//glViewport( 0, 0, aDestinationWidth, aDestinationHeight );

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//RenderSceneOnQuad( iFinalBlurredTexture );
	
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


