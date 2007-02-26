#include <gl/glew.h>
#include "FrameBufferObject.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "FloatTextProcessor.h"

void check(char* str)
{
	static GLenum errCode;
	const GLubyte *errString;
	
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error (%s): %s\n", str, errString);
		exit(1);
	}
}


FloatTextProcessor::FloatTextProcessor( float* values, int width, int height )
: iValues( values )
, iWidth( width )
, iHeight( height )
{
	iFbo = new FrameBufferObject();

	// generate texture id's
	glGenTextures(2, iTextures);

	// NOTE: use GL_TEXTURE_RECTANGLE_ARB in order to specify coords
	// from [0, height] x [0, width]
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, iTextures[0]);

	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// NOTE: as opposed to what they say in the extension description of 
	// GL_TEXTURE_RECTANGLE_ARB, ATI seems to not support 
	// the GL_REPEAT for wrapping modes
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,  // Need to use texture rectangle to get GPU-side floats
		0,						// Mipmap level 0
		GL_RGB_FLOAT32_ATI,		// Store on GPU as 3-component float (internalFormat)
		iWidth,					// width
		iHeight,				// height
		0,						// border
		GL_LUMINANCE,			// Source data is 1-component
		GL_FLOAT,				// Source data is floating point numbers
		iValues);				// Texture data

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, iTextures[1]);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// The iWidth/2 x iHeight/2 is enough for the rest of the generated textures
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB_FLOAT32_ATI, 
		iWidth, iHeight, 0, GL_LUMINANCE, GL_FLOAT, 0);

	// Shader misc.
	iShaderProgram			= new ShaderProgram();
	iInputTextureUniform	= new ShaderUniformValue<int>();
	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, "./shader/max.frag");
	iInputTextureUniform->setValue( 0 );
	iInputTextureUniform->setName("inputTexture");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->addUniformObject( iInputTextureUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}

void FloatTextProcessor::processData()
{
	int curReadTex = 0;
	int curWriteTex = 1;

	// the current texture target dimensions
	int targetWidth	= iWidth / 2;
	int targetHeight = iHeight / 2;

	// enable the texture rectangle
	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	// bind it and specify to which color attachment we 
	// want to draw to (always the same - textures permute)
	iFbo->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	iShaderProgram->useProgram();

	for (int i = 0; i < 3/*targetWidth > 1*/; i++)
	{
		curReadTex	= i % 2;
		curWriteTex = (i+1) % 2;
	
		// Don't forget to clean the COLOR/DEPTH buffers
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// define the viewport to be 1/4 of the previous texture
		// the fragment shader will output 1/4 of the data
		//glViewport(0, 0, targetWidth, targetHeight);
		glViewport(0, 0, iWidth, iHeight);

		// attach the current write texture: it should be the original one,
		// or the previously written one
		iFbo->attachTexture(iTextures[ curWriteTex ], GL_COLOR_ATTACHMENT0_EXT, 
			GL_TEXTURE_RECTANGLE_ARB, 0);

		// and finally, draw, and do the computations on the fragment shader
		renderSceneOnQuad(iTextures[curReadTex], GL_TEXTURE_RECTANGLE_ARB, 
			targetWidth, targetHeight);

		printf("Reading from texture id: %d \n", iTextures[ curReadTex ]);
		printf("Writing to texture id: %d \n", iTextures[ curWriteTex ]);

		targetWidth		/= 2;
		targetHeight	/= 2;
	}

	iShaderProgram->disableProgram();

	const int side = 1;
	GLfloat retVal[side*side*4];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, side, side, GL_RGBA, GL_FLOAT, &retVal);

	// we can draw back to screen again, if we want :)
	iFbo->unbind();
}

void FloatTextProcessor::renderSceneOnQuad(GLuint textureId, 
					   GLenum target, int width, int height)
{
	/************************************************************************/
	/* CAN PASS THIS TO OUTSIDE, MORE EFFICIENT TO JUST CALL ONCE           */
	/************************************************************************/

	{
			glMatrixMode( GL_PROJECTION );
			glPushMatrix();
			glLoadIdentity();
			//gluOrtho2D( -1.0f, 1.0f, -1.0f, 1.0f );
			gluOrtho2D( 0.0f, iWidth, 0.0f, iHeight);
			//gluOrtho2D( 0.0f, iWidth, 0.0f, iHeight );
		
			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadIdentity();
			glEnable( target );
			
	}

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(target, textureId);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(target, textureId);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(target, textureId);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(target, textureId);

	glBindTexture(target, textureId);
	glBegin(GL_QUADS);
	//glMultiTexCoord2f(GL_TEXTURE0, -0.5, -0.5);
	//glMultiTexCoord2f(GL_TEXTURE1,  0.5, -0.5);
	//glMultiTexCoord2f(GL_TEXTURE2, -0.5,  0.5);
	//glMultiTexCoord2f(GL_TEXTURE3,  0.5,  0.5);
	////glVertex2f( -1.0f, -1.0f );
	//glVertex2f( 0, 0 );

	//glMultiTexCoord2f(GL_TEXTURE0, 2*static_cast<GLfloat>(width)-0.5, -0.5);
	//glMultiTexCoord2f(GL_TEXTURE1, 2*static_cast<GLfloat>(width)+0.5, -0.5);
	//glMultiTexCoord2f(GL_TEXTURE2, 2*static_cast<GLfloat>(width)-0.5,  0.5);
	//glMultiTexCoord2f(GL_TEXTURE3, 2*static_cast<GLfloat>(width)+0.5,  0.5);
	////glVertex2f( 1.0f, -1.0f );
	//glVertex2f( width, 0 );

	//glMultiTexCoord2f(GL_TEXTURE0, 2*static_cast<GLfloat>(width)-0.5, 
	//	2*static_cast<GLfloat>(height)-0.5);
	//glMultiTexCoord2f(GL_TEXTURE1, 2*static_cast<GLfloat>(width)+0.5, 
	//	2*static_cast<GLfloat>(height)-0.5);
	//glMultiTexCoord2f(GL_TEXTURE2, 2*static_cast<GLfloat>(width)-0.5, 
	//	2*static_cast<GLfloat>(height)+0.5);
	//glMultiTexCoord2f(GL_TEXTURE3, 2*static_cast<GLfloat>(width)+0.5, 
	//	2*static_cast<GLfloat>(height)+0.5);
	////glVertex2f( 1.0f, 1.0f );
	//glVertex2f( width, height );

	//glMultiTexCoord2f(GL_TEXTURE0, 2*static_cast<GLfloat>(width)-0.5, 
	//	2*static_cast<GLfloat>(height)-0.5);
	//glMultiTexCoord2f(GL_TEXTURE1, 2*static_cast<GLfloat>(width)+0.5, 
	//	2*static_cast<GLfloat>(height)-0.5);
	//glMultiTexCoord2f(GL_TEXTURE2, 2*static_cast<GLfloat>(width)-0.5, 
	//	2*static_cast<GLfloat>(height)+0.5);
	//glMultiTexCoord2f(GL_TEXTURE3, 2*static_cast<GLfloat>(width)+0.5, 
	//	2*static_cast<GLfloat>(height)+0.5);
	////glVertex2f( -1.0f, 1.0f );
	//glVertex2f( 0, height );

	/*
	 *
	 */

	//glTexCoord2f(-0.5, -0.5); 
	//glVertex3f(-1, -1, -0.5f);

	//glTexCoord2f(2*width - 0.5, 0);
	//glVertex3f( 1, -1, -0.5f);

	//glTexCoord2f(2*width - 0.5, 2*height - 0.5);
	//glVertex3f( 1,  1, -0.5f);

	//glTexCoord2f(0, 2*height - 0.5);
	//glVertex3f(-1,  1, -0.5f);


	glVertex2f(0.0, 0.0);
	glVertex2f(width, 0.0);
	glVertex2f(width, height);
	glVertex2f(0.0, height);
	
	glEnd();
	glPopMatrix();

	///************************************************************************/
	///* CAN PASS THIS TO OUTSIDE, MORE EFFICIENT TO JUST CALL ONCE           */
	///************************************************************************/
	{
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
	}
	glDisable( target );
}

FloatTextProcessor::~FloatTextProcessor(void)
{
	delete iFbo;
	delete iFragmentShader;
	delete iShaderProgram;
}
