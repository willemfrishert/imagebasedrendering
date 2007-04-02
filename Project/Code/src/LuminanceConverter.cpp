#include <gl/glew.h>
#include "FrameBufferObject.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderUniformVector.h"
#include "ShaderProgram.h"
#include "LuminanceConverter.h"

LuminanceConverter::LuminanceConverter(int width, int height, 
									   int minExponent, int maxExponent)
: iWidth( width )
, iHeight( height )
, iMinExponent( minExponent )
, iMaxExponent( maxExponent )
{
	// generate texture id's
	glGenTextures(1, &iLuminanceTexture);
	setupTexture(iLuminanceTexture, 0);

	// create FBO
	iFbo = new FrameBufferObject();
	iFbo->bind();

	iFbo->attachTexture(iLuminanceTexture, GL_COLOR_ATTACHMENT0_EXT, 
		GL_TEXTURE_2D, 0);

	FrameBufferObject::unbind();

	// Shader misc.
	initShaders();
}

LuminanceConverter::~LuminanceConverter(void)
{
}

void LuminanceConverter::setupTexture(GLuint textureId, const GLvoid* data)
{
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D,  // Need to use texture rectangle to get GPU-side floats
		0,						// Mipmap level 0
		GL_RGBA,				// Store on GPU as 3-component float (internalFormat)
		iWidth,					// width
		iHeight,				// height
		0,						// border
		GL_RGBA,				// Source data is 1-component
		GL_FLOAT,				// Source data is floating point numbers
		data);					// Texture data
}

void LuminanceConverter::initShaders( )
{
	iShaderProgram			= new ShaderProgram();
	iInputTextureUniform	= new ShaderUniformValue<int>();
	iScaleBias				= new ShaderUniformVector<float>(VEC2);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, "./shader/luminance.frag");

	// Uniforms
	iInputTextureUniform->setValue( 0 );
	iInputTextureUniform->setName("inputTexture");

	// compute scaleBias:
	//float sb[2];
	//sb[0] = static_cast<float>((iMaxExponent - iMinExponent)); // scale
	//sb[1] = static_cast<float>(iMinExponent); // bias
	float sb[2];
	sb[0] = static_cast<float>(255.0f); // scale
	sb[1] = static_cast<float>(-128.0f); // bias

	iScaleBias->setValues(sb);
	iScaleBias->setName("scaleBias");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->addUniformObject( iInputTextureUniform );
	//iShaderProgram->addUniformObject( iScaleBias );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}

void LuminanceConverter::renderSceneOnQuad(GLuint textureId, GLenum target)
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

/**
 * @return the luminance texture id
 */
GLuint LuminanceConverter::processData(GLuint inputTextureId)
{
	//glViewport(0, 0, iWidth, iHeight);

	// enable the texture rectangle
	glEnable(GL_TEXTURE_2D);

	// bind it and specify to which color attachment we 
	// want to draw to (always the same - textures permute)
	iFbo->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iShaderProgram->useProgram();
	{
		// and finally, draw, and do the computations on the fragment shader
		renderSceneOnQuad(inputTextureId, GL_TEXTURE_2D);

	}
	iShaderProgram->disableProgram();

	// we can draw back to screen again, if we want :)
	iFbo->unbind();

	return iLuminanceTexture;
}