#include <GL/glew.h>
#include "FrameBufferObject.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "GPUParallelReductor.h"

GLenum GPU_REDUCTOR_TEXTURE_TARGET = GL_TEXTURE_2D;

GPUParallelReductor::GPUParallelReductor( float* values, int width, int height, string fragmentShaderFilename, 
									   GLint texInternalFormat, GLenum texFormat )
: iWidth( width )
, iHeight( height )
, internalFormat( texInternalFormat )
, iOriginalTexture( 0 )
, format( texFormat )
{
	iFbo = new FrameBufferObject();

	// generate the necessary textures: 3 (1 to store the original + 2 for ping-pong)	
	generateTextures(values);

	// Shader misc.
	initShaders( fragmentShaderFilename );
	
}

GPUParallelReductor::GPUParallelReductor(int width, int height, 
									   string fragmentShaderFilename, GLint texInternalFormat, GLenum texFormat)
: iWidth( width )
, iHeight( height )
, iOriginalTexture( 0 )
, internalFormat( texInternalFormat )
, format( texFormat )
{
	iFbo = new FrameBufferObject();

	// generate the necessary textures: 3 (1 to store the original + 2 for ping-pong)	
	generateTextures(NULL);

	// Shader misc.
	initShaders( fragmentShaderFilename );
}

void GPUParallelReductor::initShaders( string fragmentShaderFilename )
{
	iShaderProgram			= new ShaderProgram();
	iInputTextureUniform	= new ShaderUniformValue<int>();
	iTexHeightUniform		= new ShaderUniformValue<float>();

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, fragmentShaderFilename);
	
	// hardcoded: doesn't change
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, "./shader/computecoords.vert");
	
	iInputTextureUniform->setValue( 0 );
	iInputTextureUniform->setName("inputTexture");

	iTexHeightUniform->setValue(iHeight);
	iTexHeightUniform->setName("height");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->addUniformObject( iInputTextureUniform );
	iShaderProgram->addUniformObject( iTexHeightUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}

void GPUParallelReductor::generateTextures( float* originalData )
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

void GPUParallelReductor::setupTexture(GLuint textureId, const GLvoid* data)
{
	// NOTE: use GPU_REDUCTOR_TEXTURE_TARGET in order to specify coords
	// from [0, height] x [0, width]
	glBindTexture(GPU_REDUCTOR_TEXTURE_TARGET, textureId);

	glTexParameteri(GPU_REDUCTOR_TEXTURE_TARGET, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GPU_REDUCTOR_TEXTURE_TARGET, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// NOTE: as opposed to what they say in the extension description of 
	// GPU_REDUCTOR_TEXTURE_TARGET, ATI seems to not support 
	// the GL_REPEAT for wrapping modes
	glTexParameteri(GPU_REDUCTOR_TEXTURE_TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GPU_REDUCTOR_TEXTURE_TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/*
	 * NOTE: The internalFormat is, by default, GL_RGB_FLOAT32_ATI, because the buffer
	 * might be used to accumulate large values and FP16 can only reach 65536
	 */
	glTexImage2D(GPU_REDUCTOR_TEXTURE_TARGET,  // Need to use texture rectangle to get GPU-side floats
		0,						// Mipmap level 0
		internalFormat,			// Store on GPU as 3-component float (internalFormat)
		iWidth,					// width
		iHeight,				// height
		0,						// border
		format,					// Source data is 1-component
		GL_FLOAT,				// Source data is floating point numbers
		data);					// Texture data
}


void GPUParallelReductor::setOriginalTexture(GLuint originalTexId)
{
	this->iOriginalTexture = originalTexId;
}

/**
 * @description just calls the processData() method with
 * the iOriginalTexture that was set previously
 * @param result
 */
void GPUParallelReductor::processData( float* result )
{
	if ( this->iOriginalTexture != 0)
	{
		processData(this->iOriginalTexture, result);
	}
}

/**
 * @description Processes the Parallel Reduction algorithm given the passed shader
 * NOTE: result must have allocated space for sizeof(float) * 4
 * @return the result of the algorithm
 */
void GPUParallelReductor::processData( GLuint originalTexId, float* result )
{
	glViewport(0, 0, iWidth, iHeight);

	int curReadTex = 0;
	int curWriteTex = 1;

	// the current texture target dimensions
	int targetWidth	= iWidth / 2;
	int targetHeight = iHeight / 2;

	// enable the texture rectangle
	glEnable(GPU_REDUCTOR_TEXTURE_TARGET);

	// bind it and specify to which color attachment we 
	// want to draw to (always the same - textures permute)
	iFbo->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	iShaderProgram->useProgram();

	// First, do the first pass of the reduction from the original to one of the 
	// ping-pong textures so we can continue using them without corrupting the original
	iFbo->attachTexture(iTextures[ 0 ], GL_COLOR_ATTACHMENT0_EXT, 
		GPU_REDUCTOR_TEXTURE_TARGET, 0);

	// and finally, draw, and do the computations on the fragment shader
	renderSceneOnQuad(originalTexId, GPU_REDUCTOR_TEXTURE_TARGET, 
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
			GPU_REDUCTOR_TEXTURE_TARGET, 0);

		// and finally, draw, and do the computations on the fragment shader
		renderSceneOnQuad(iTextures[curReadTex], GPU_REDUCTOR_TEXTURE_TARGET, 
			targetWidth, targetHeight);
	}

	iShaderProgram->disableProgram();

	const int side = 1;
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, side, side, GL_RGBA, GL_FLOAT, result);

	// we can draw back to screen again, if we want :)
	FrameBufferObject::unbind();
}

void GPUParallelReductor::renderSceneOnQuad(GLuint textureId, 
					   GLenum target, int width, int height)
{
	glEnable(target);
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


GPUParallelReductor::~GPUParallelReductor(void)
{
	delete iFbo;
	delete iFragmentShader;
	delete iShaderProgram;
	glDeleteTextures(2, iTextures);
}
