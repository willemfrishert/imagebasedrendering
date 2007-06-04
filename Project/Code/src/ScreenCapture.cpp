#include <gl/glew.h>
#include <windows.h>
#include "FrameBufferObject.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "ScreenCapture.h"
#include "CodecRGBE.h"

static GLenum screenCaptureTexTarget = GL_TEXTURE_2D;

ScreenCapture::ScreenCapture(int width, int height)
: iShaderProgram( NULL )
{
	// Setup Scene Texture
	iCaptureFBO = new FrameBufferObject();

	glGenTextures(1, &iCaptureTexture);
	setupTexture(iCaptureTexture, NULL, width, height);
	iCaptureFBO->bind();
	iCaptureFBO->attachTexture(iCaptureTexture, GL_COLOR_ATTACHMENT0_EXT, screenCaptureTexTarget, 0);

	iCaptureFBO->attachDepthRenderBuffer(width, height);
	FrameBufferObject::unbind();

#ifdef _DEBUG
	iShaderProgram			= new ShaderProgram();
	iInputTextureUniform	= new ShaderUniformValue<int>();

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, "./shader/cubemap.frag");

	// Uniforms
	iInputTextureUniform->setValue( 0 );
	iInputTextureUniform->setName("cubeMap");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iInputTextureUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
#endif // _DEBUG

}

ScreenCapture::~ScreenCapture(void)
{
	if ( iShaderProgram )
	{
		delete iShaderProgram;
		delete iFragmentShader;
		delete iInputTextureUniform;
	}
}

void ScreenCapture::changeResolution(int width, int height)
{
	// TODO: glDeleteTexture
	// generate new texture
}

void ScreenCapture::startCapture()
{
	// Setup FBO to render the scene to a texture
	iCaptureFBO->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//iShaderProgram->useProgram();
}


/**
 */
GLuint ScreenCapture::stopCapture()
{
	//iShaderProgram->disableProgram();

	FrameBufferObject::unbind();

	return iCaptureTexture;
}

/**
 */
void ScreenCapture::renderToScreen()
{
	FrameBufferObject::unbind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/************************************************************************/
	/* ON PURPOSE: NEEDS SIZE OF TEXTURE IF USING RECTANGLES                */
	/************************************************************************/
	exit(0);

	renderSceneOnQuad(iCaptureTexture, screenCaptureTexTarget);
}

void ScreenCapture::setupTexture(GLuint textureId, const GLvoid* data, int width, int height)
{
	// NOTE: use GL_TEXTURE_RECTANGLE_ARB in order to specify coords
	// from [0, height] x [0, width]
	glBindTexture(screenCaptureTexTarget, textureId);

	glTexParameteri(screenCaptureTexTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(screenCaptureTexTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// NOTE: as opposed to what they say in the extension description of 
	// GL_TEXTURE_RECTANGLE_ARB, ATI seems to not support 
	// the GL_REPEAT for wrapping modes
	glTexParameteri(screenCaptureTexTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(screenCaptureTexTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(screenCaptureTexTarget, 0, GL_RGB16, width, 
		height, 0, GL_RGB, GL_FLOAT, data);
}

void ScreenCapture::renderSceneOnQuad(GLuint textureId, GLenum target)
{
	glEnable( target );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	{
		glLoadIdentity();
		gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f);

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		{
			glLoadIdentity();

			glBindTexture(target, textureId);
			glBegin(GL_QUADS);
			{
				glTexCoord2i(0, 0);
				glVertex2f(0.0f, 0.0f);
				
				glTexCoord2i(1, 0);
				glVertex2f(1.0f, 0.0f);
				
				glTexCoord2i(1, 1);
				glVertex2f(1.0f, 1.0f);
				
				glTexCoord2i(0, 1);
				glVertex2f(0.0f, 1.0f);
			}
			glEnd();
		}
		glPopMatrix();

		glMatrixMode( GL_PROJECTION );
	}
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );

	glDisable( target );
}