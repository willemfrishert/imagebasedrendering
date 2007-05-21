#include "BlurTexture.h"
#include "OpenGLUtility.h"

BlurTexture::BlurTexture()
: iOriginalImageSize( ETextureSize512 )
{
	Init();
}

BlurTexture::~BlurTexture()
{
	delete iOriginalFBO;
	glDeleteTextures(1, &iOriginalTexAttachment.id);

	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		glDeleteTextures(1, &iHorizBlurredTexAttachment[i].id);
		glDeleteTextures(1, &iVertBlurredTexAttachment[i].id);
		delete iIntermediateFBO[i];
	}
}

void BlurTexture::Init()
{
	//iMipmapSize[0] = ETextureSize256;
	//iMipmapSize[1] = ETextureSize128;
	//iMipmapSize[2] = ETextureSize64;
	//iMipmapSize[3] = ETextureSize32;

	iMipmapSize[0] = ETextureSize128;
	iMipmapSize[1] = ETextureSize64;
	iMipmapSize[2] = ETextureSize32;
	iMipmapSize[3] = ETextureSize16;

	//iMipmapSize[0] = ETextureSize128;
	//iMipmapSize[1] = ETextureSize32;
	//iMipmapSize[2] = ETextureSize16;
	//iMipmapSize[3] = ETextureSize8;

	iBlurPasses[0] = 1;
	iBlurPasses[1] = 4;
	iBlurPasses[2] = 7;
	iBlurPasses[3] = 10;

	InitFramebufferObject();
	InitTextures();
	InitShaders();
}

void BlurTexture::InitTextures()
{
	// Set up originalFBO with one texture

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexAttachment.id);
	iOriginalTexAttachment.target = GL_TEXTURE_2D;
	iOriginalTexAttachment.attachment = GL_COLOR_ATTACHMENT0_EXT;
	iOriginalTexAttachment.mipmapLevel = 0;

	iOriginalFBO->bind();

	glBindTexture(iOriginalTexAttachment.target, iOriginalTexAttachment.id);
	glTexParameterf(iOriginalTexAttachment.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(iOriginalTexAttachment.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(iOriginalTexAttachment.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(iOriginalTexAttachment.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(iOriginalTexAttachment.target, 0, GL_RGB16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenerateMipmapEXT(iOriginalTexAttachment.target);

	iOriginalFBO->attachTextureAttachment(iOriginalTexAttachment);
	iOriginalFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);


	// setup N intermediate FBOs. One FBO per mipmap level. Each FBO contains 2 textures used 
	// to generate downscaled blurred images of the real scene.
	for (int i = 0; i < KNumberOfBlurLevels; i++)
	{
		iIntermediateFBO[i]->bind();

		glGenTextures(1, &iHorizBlurredTexAttachment[i].id);
		iHorizBlurredTexAttachment[i].target = GL_TEXTURE_2D;
		iHorizBlurredTexAttachment[i].attachment = GL_COLOR_ATTACHMENT0_EXT;
		iHorizBlurredTexAttachment[i].mipmapLevel = 0;

		glBindTexture(iHorizBlurredTexAttachment[i].target, iHorizBlurredTexAttachment[i].id);
		glTexParameterf(iHorizBlurredTexAttachment[i].target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(iHorizBlurredTexAttachment[i].target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(iHorizBlurredTexAttachment[i].target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(iHorizBlurredTexAttachment[i].target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(iHorizBlurredTexAttachment[i].target, 0, GL_RGB16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		iIntermediateFBO[i]->attachTextureAttachment(iHorizBlurredTexAttachment[i]);

		glGenTextures(1, &iVertBlurredTexAttachment[i].id);
		iVertBlurredTexAttachment[i].target = GL_TEXTURE_2D;
		iVertBlurredTexAttachment[i].attachment = GL_COLOR_ATTACHMENT1_EXT;
		iVertBlurredTexAttachment[i].mipmapLevel = 0;

		iFinalBlurredTexture[i] = iVertBlurredTexAttachment[i].id;

		glBindTexture(iVertBlurredTexAttachment[i].target, iVertBlurredTexAttachment[i].id);
		glTexParameterf(iVertBlurredTexAttachment[i].target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(iVertBlurredTexAttachment[i].target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(iVertBlurredTexAttachment[i].target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(iVertBlurredTexAttachment[i].target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(iVertBlurredTexAttachment[i].target, 0, GL_RGB16, iMipmapSize[i], iMipmapSize[i], 0, GL_RGBA, GL_FLOAT, NULL);

		iIntermediateFBO[i]->attachTextureAttachment(iVertBlurredTexAttachment[i]);

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

	iHorizontalShaderProgram = new ShaderProgram();
	iVerticalShaderProgram = new ShaderProgram();
	iHorizontalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iHorizontalShaderProgram->attachShader( *(iHorizontalBlurFragmentShader) );

	iVerticalShaderProgram->attachShader( *iCodecRGBEFragmentShader );
	iVerticalShaderProgram->attachShader( *(iVerticalBlurFragmentShader) );

	iMipmapSizeUniform.set( "mipmapSize", iMipmapSize[0] );
	iVerticalTextureSizeUniform.set("mipmapSize", iMipmapSize[0] );

	iHorizontalShaderProgram->addUniformObject( &iMipmapSizeUniform );
	iVerticalShaderProgram->addUniformObject( &iVerticalTextureSizeUniform );

	iHorizBlurWeight1Uniform.set("blurWeight1", 0.375f);
	iHorizBlurWeight2Uniform.set("blurWeight2",	0.250f);
	iHorizBlurWeight3Uniform.set("blurWeight3",	0.062f);
	iVertBlurWeight1Uniform.set("blurWeight1", 0.375f);
	iVertBlurWeight2Uniform.set("blurWeight2", 0.250f);
	iVertBlurWeight3Uniform.set("blurWeight3", 0.062f);

	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight1Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight2Uniform);
	iHorizontalShaderProgram->addUniformObject( &iHorizBlurWeight3Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight1Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight2Uniform);
	iVerticalShaderProgram->addUniformObject( &iVertBlurWeight3Uniform);

	iHorizontalShaderProgram->buildProgram();
	iVerticalShaderProgram->buildProgram();
}

void BlurTexture::processData(GLuint aProcessTex, GLuint* aBlurredTextures)
{
	// setup view port
	glGetIntegerv( GL_VIEWPORT, iOldViewPort );	
	glViewport( 0, 0, iOriginalImageSize, iOriginalImageSize );

	// make a copy of the initial texture
	iOriginalFBO->bind();
	glDrawBuffer(iOriginalTexAttachment.attachment);
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		RenderSceneOnQuad(aProcessTex, iOriginalImageSize );
	}
	FrameBufferObject::unbind();

	// generate mipmaps
	glBindTexture( iOriginalTexAttachment.target, iOriginalTexAttachment.id );
	glGenerateMipmapEXT( iOriginalTexAttachment.target );

	for (int textureCounter = 0 ; textureCounter < KNumberOfBlurLevels; textureCounter++)
	{
		BlurMipmap( iOriginalTexAttachment.id, textureCounter, iMipmapSize[textureCounter], iBlurPasses[textureCounter] );
	}

	// reset viewport
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	aBlurredTextures[0] = iFinalBlurredTexture[0];
	aBlurredTextures[1] = iFinalBlurredTexture[1];
	aBlurredTextures[2] = iFinalBlurredTexture[2];
	aBlurredTextures[3] = iFinalBlurredTexture[3];
}

void BlurTexture::BlurMipmap( GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aBlurPasses  )
{
	// setup mipmap viewport size
	GLint oldViewPort[4];
	glGetIntegerv( GL_VIEWPORT, oldViewPort );
	glViewport(0, 0, aMipmapSize, aMipmapSize);

	// bind the intermediate FBO for pingponging
	iIntermediateFBO[aCounter]->bind();

	for (int i=0; i < aBlurPasses; i++)
	{
		// render the horizontal blurred texture using the original image texture or vertical image
		glDrawBuffer( iHorizBlurredTexAttachment[aCounter].attachment );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		iMipmapSizeUniform.setValue( aMipmapSize );
		iHorizontalShaderProgram->useProgram();
		{
			if (i == 0)
			{
				RenderSceneOnQuad( aTextureID, aMipmapSize );
			}
			else
			{
				RenderSceneOnQuad( iVertBlurredTexAttachment[aCounter].id, aMipmapSize );
			}
		}
		iHorizontalShaderProgram->disableProgram();

		//GLfloat pixels[8*8*4];
		//glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		//glReadPixels(0,0,aWidth, aHeight, GL_RGBA, GL_FLOAT, pixels);

		// render the vertical texture using the horizontal texture
		glDrawBuffer( iVertBlurredTexAttachment[aCounter].attachment );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		iVerticalTextureSizeUniform.setValue( aMipmapSize );
		iVerticalShaderProgram->useProgram();
		{
			RenderSceneOnQuad( iHorizBlurredTexAttachment[aCounter].id, aMipmapSize );
		}
		iVerticalShaderProgram->disableProgram();
	}

	// render back to screen
	FrameBufferObject::unbind();
}

void BlurTexture::RenderSceneOnQuad( GLuint aTextureID, GLuint aTextureSize )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, aTextureSize, 0, aTextureSize );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, aTextureID );

	glBegin(GL_QUADS);

	glTexCoord2f( 0, 0 );
	glVertex2f(           0,           0 );

	glTexCoord2f( 1, 0 );
	glVertex2f( aTextureSize,           0 );

	glTexCoord2f( 1, 1 );
	glVertex2f( aTextureSize, aTextureSize );

	glTexCoord2f( 0, 1 );
	glVertex2f(           0, aTextureSize );
	glEnd();

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}

