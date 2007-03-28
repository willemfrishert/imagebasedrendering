#include <gl/glew.h>
#include "FrameBufferObject.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "FloatTextProcessor.h"

inline void check(char* str)
{
	static GLenum errCode;
	const GLubyte *errString;
	
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error (%s): %s\n", str, errString);
		exit(1);
	}
}


FloatTextProcessor::FloatTextProcessor( float* values, int width, int height, string fragmentShaderFilename, 
									   GLint texInternalFormat, GLenum texFormat )
: iWidth( width )
, iHeight( height )
, internalFormat( texInternalFormat )
, format( texFormat )
{
	iFbo = new FrameBufferObject();

	// generate the necessary textures: 3 (1 to store the original + 2 for ping-pong)	
	generateTextures(values);

	// Shader misc.
	initShaders( fragmentShaderFilename );
	
}

FloatTextProcessor::FloatTextProcessor(GLuint originalTexId, int width, int height, 
									   string fragmentShaderFilename, GLint texInternalFormat, GLenum texFormat)
: iWidth( width )
, iHeight( height )
, iOriginalTexture( originalTexId )
, internalFormat( texInternalFormat )
, format( texFormat )
{
	iFbo = new FrameBufferObject();

	// generate the necessary textures: 3 (1 to store the original + 2 for ping-pong)	
	generateTextures(NULL);

	// Shader misc.
	initShaders( fragmentShaderFilename );
}

void FloatTextProcessor::initShaders( string fragmentShaderFilename )
{
	iShaderProgram			= new ShaderProgram();
	iInputTextureUniform	= new ShaderUniformValue<int>();

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, fragmentShaderFilename);
	
	// hardcoded: doesn't change
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, "./shader/computecoords.vert");
	
	iInputTextureUniform->setValue( 0 );
	iInputTextureUniform->setName("inputTexture");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->addUniformObject( iInputTextureUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}

void FloatTextProcessor::generateTextures( float* originalData )
{
	// generate texture id's
	glGenTextures(2, iTextures);

	setupTexture(iTextures[0], 0);
	setupTexture(iTextures[1], 0);

	if ( originalData != NULL)
	{
		glGenTextures(1, &iOriginalTexture);
		setupTexture(iOriginalTexture, originalData);
	}
}

void FloatTextProcessor::setupTexture(GLuint textureId, const GLvoid* data)
{
	// NOTE: use GL_TEXTURE_RECTANGLE_ARB in order to specify coords
	// from [0, height] x [0, width]
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureId);

	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// NOTE: as opposed to what they say in the extension description of 
	// GL_TEXTURE_RECTANGLE_ARB, ATI seems to not support 
	// the GL_REPEAT for wrapping modes
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,  // Need to use texture rectangle to get GPU-side floats
		0,						// Mipmap level 0
		internalFormat,			// Store on GPU as 3-component float (internalFormat)
		iWidth,					// width
		iHeight,				// height
		0,						// border
		format,					// Source data is 1-component
		GL_FLOAT,				// Source data is floating point numbers
		data);					// Texture data
}

/**
 * @description Processes the Parallel Reduction algorithm given the passed shader
 * @return the result of the algorithm
 */
float FloatTextProcessor::processData()
{
	glViewport(0, 0, iWidth, iHeight);

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

	// First, do the first pass of the reduction from the original to one of the 
	// ping-pong textures so we can continue using them without corrupting the original
	iFbo->attachTexture(iTextures[ 0 ], GL_COLOR_ATTACHMENT0_EXT, 
		GL_TEXTURE_RECTANGLE_ARB, 0);

	// and finally, draw, and do the computations on the fragment shader
	renderSceneOnQuad(iOriginalTexture, GL_TEXTURE_RECTANGLE_ARB, 
		targetWidth, targetHeight);

	for (int i = 0; targetWidth > 1; i++)
	{
		curReadTex	= i % 2;
		curWriteTex = (i+1) % 2;

		targetWidth		/= 2;
		targetHeight	/= 2;

		// attach the current write texture: it should be the original one,
		// or the previously written one
		iFbo->attachTexture(iTextures[ curWriteTex ], GL_COLOR_ATTACHMENT0_EXT, 
			GL_TEXTURE_RECTANGLE_ARB, 0);

		// and finally, draw, and do the computations on the fragment shader
		renderSceneOnQuad(iTextures[curReadTex], GL_TEXTURE_RECTANGLE_ARB, 
			targetWidth, targetHeight);
	}

	iShaderProgram->disableProgram();

	const int side = 1;
	GLfloat result[side*side*4];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, side, side, GL_RGBA, GL_FLOAT, &result);

	// we can draw back to screen again, if we want :)
	FrameBufferObject::unbind();

	// any component will do: (V, V, V, 1)
	return result[ 0 ];
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
			gluOrtho2D( 0.0f, iWidth, 0.0f, iHeight);
		
			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadIdentity();
			
	}

	glBindTexture(target, textureId);
	glBegin(GL_QUADS);

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
	glDeleteTextures(2, iTextures);
}
