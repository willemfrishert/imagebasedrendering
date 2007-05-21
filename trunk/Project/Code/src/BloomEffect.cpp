#include "BloomEffect.h"
#include "Utility.h"
#include <vector>
#include "CodecRGBE.h"

using namespace std;

BloomEffect::BloomEffect()
: iOriginalImageSize( 512 )
, iCycleTextureOutput( 0 )
{
	Init();
}

BloomEffect::~BloomEffect()
{
	delete iOriginalFBO;
	delete iBlendedFBO;

	delete iBrightpassShaderProgram;
	delete iBlenderShaderProgram;

	delete iBrightpassFragmentShader;

	delete iLuminanceConverter;
	delete iGPUParallelReductor;
	delete iBlurTexture;

	glDeleteTextures(1, &iOriginalTexture);
	glDeleteTextures(1, &iBrightpassTexture);
	glDeleteTextures(1, &iBlendedTexture);
}

void BloomEffect::ProcessNormalKeys( unsigned char key, int x, int y )
{	
	switch( key )
	{
	case 'b':
		  iCycleTextureOutput = (iCycleTextureOutput + 1) % 7;
		break;
	case 'B':
		iCycleTextureOutput = (iCycleTextureOutput + 6) % 7;
		break;
	default:
		break;
	}
}
void BloomEffect::ProcessCursorKeys( int key, int x, int y )
{

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
	iTextureLayers = new GLuint[5];

	iOriginalFBO->bind();

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iOriginalTexture);

	glBindTexture(GL_TEXTURE_2D, iOriginalTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	iOriginalFBO->attachTexture(iOriginalTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	iTextureLayers[0] = iOriginalTexture;

	// initialize texture that will store the framebuffer image
	glGenTextures(1, &iBrightpassTexture);

	glBindTexture(GL_TEXTURE_2D, iBrightpassTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);

	iOriginalFBO->attachTexture(iBrightpassTexture, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, 0);

	iOriginalFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

	FrameBufferObject::unbind();

	iBlendedFBO->bind();

	glGenTextures(1, &iBlendedTexture);

	glBindTexture(GL_TEXTURE_2D, iBlendedTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, iOriginalImageSize, iOriginalImageSize, 0, GL_RGBA, GL_FLOAT, NULL);
	
	iBlendedFBO->attachTexture(iBlendedTexture, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0);

	iBlendedFBO->attachDepthRenderBuffer(iOriginalImageSize, iOriginalImageSize);

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
	InitBrightpassShader();
	InitBlendShader();
}


void BloomEffect::InitBrightpassShader()
{
	iBrightpassShaderProgram = new ShaderProgram();
	iBrightpassFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/brightpass.frag");

	iBrightpassShaderProgram->attachShader( *iBrightpassFragmentShader );
	iBrightpassShaderProgram->attachShader( *CodecRGBE::getShaderObject() );

	iLogAvgLum.setValue(0.0f);
	iLogAvgLum.setName("logAvgLum");

	iBrightpassShaderProgram->addUniformObject( &iLogAvgLum );
	iBrightpassShaderProgram->buildProgram();
}

void BloomEffect::InitBlendShader()
{
	iBlenderShaderProgram = new ShaderProgram();
	iBlenderFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/bloomBlender.frag");

	iBlenderShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iBlenderShaderProgram->attachShader( *iBlenderFragmentShader );

	iBlenderOriginalTextureUniform.set( "originalTexture", 0 );
	iBlenderBlur1TextureUniform.set( "blurTexture0", 1 );
	iBlenderBlur2TextureUniform.set( "blurTexture1", 2 );
	iBlenderBlur3TextureUniform.set( "blurTexture2", 3 );
	iBlenderBlur4TextureUniform.set( "blurTexture3", 4 );

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


	iOriginalFBO->bind();

	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	iLogAvgLum.setValue( logAverage );

	iBrightpassShaderProgram->useProgram();
	{
		OpenGLUtility::RenderSceneOnQuadMultiTex( &iOriginalTexture, 1 );
	}
	iBrightpassShaderProgram->disableProgram();

	//const int width = 4;
	//const int height = 4;
	//float result[width*height*4];
	//glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
	//glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, result);
	FrameBufferObject::unbind();

	iBlurTexture->processData(iBrightpassTexture, iTextureLayers+1 );
	
	// blend the images as a final result
	glViewport( iOldViewPort[0], iOldViewPort[1], iOldViewPort[2], iOldViewPort[3] );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//RenderSceneOnQuad(iFinalBlurredTexture[0]);
	RenderBloomEffect(iTextureLayers);

	switch(iCycleTextureOutput)
	{
		case 0:
		{
			return iTextureLayers[0]; // original tex
		}
		break;
		case 1:
		{
			return iBrightpassTexture;
		}
		break;
		case 2:
		{
			return iTextureLayers[1];
		}
		break;
		case 3:
		{
			return iTextureLayers[2];
		}
		break;
		case 4:
		{
			return iTextureLayers[3];
		}
		break;
		case 5:
		{
			return iTextureLayers[4];
		}
		break;
		default:
		{
			return iBlendedTexture;
		}
	}
}

void BloomEffect::RenderBloomEffect(const GLuint* iTextureLayers)
{
	iBlendedFBO->bind();
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iBlenderShaderProgram->useProgram();
		OpenGLUtility::RenderSceneOnQuadMultiTex( iTextureLayers, 5 );
	iBlenderShaderProgram->disableProgram();
	FrameBufferObject::unbind();
}


