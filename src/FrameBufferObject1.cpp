#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <assert.h>
#include "FrameBufferObject.h"

#define CHECK_FRAMEBUFFER_STATUS() \
{\
	GLenum status; \
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
	fprintf(stderr, "%x\n", status);\
	switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
 fprintf(stderr,"framebuffer complete!\n");\
 break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
 fprintf(stderr,"framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");\
 /* you gotta choose different formats */ \
 assert(0); \
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
 fprintf(stderr,"framebuffer INCOMPLETE_ATTACHMENT\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
 fprintf(stderr,"framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
 fprintf(stderr,"framebuffer FRAMEBUFFER_DIMENSIONS\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
 fprintf(stderr,"framebuffer INCOMPLETE_FORMATS\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
 fprintf(stderr,"framebuffer INCOMPLETE_DRAW_BUFFER\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
 fprintf(stderr,"framebuffer INCOMPLETE_READ_BUFFER\n");\
 break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
 fprintf(stderr,"framebuffer BINDING_EXT\n");\
 break; \
 /* 
case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
fprintf(stderr,"framebuffer STATUS_ERROR\n");\
break; \
*/ \
 default: \
		  /* programming error; will fail on all hardware */ \
		  assert(0); \
}\
}

FrameBufferObject::FrameBufferObject(void)
{
	// generate the frambuffer object (FBO), in a similar manner as a texture
	glGenFramebuffersEXT(1, &m_FrameBufferId);

	errcheck();
}

FrameBufferObject::~FrameBufferObject(void)
{
}

/**
 * @param texture
 * @param attachment
 */
void FrameBufferObject::attachTexture( GLuint texture, GLenum attachment, GLint mipmapLevel )
{
	// bind this texture to the current framebuffer obj. as 
	// attachment 
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
		attachment, GL_TEXTURE_2D, texture, mipmapLevel);

	errcheck();

	//see if everything is OK
	CHECK_FRAMEBUFFER_STATUS()
}

void FrameBufferObject::attachDepthBuffer(GLuint depthBufferId)
{
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
	//	GL_DEPTH_ATTACHMENT_EXT,
	//	GL_RENDERBUFFER_EXT, depthBufferId);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
		GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBufferId, 0);

	errcheck();

	//see if everything is OK
	CHECK_FRAMEBUFFER_STATUS()
}

void FrameBufferObject::attachDepthRenderBuffer(GLuint aWidth, GLuint aHeight)
{
	GLuint depthBufferId;
	glGenRenderbuffersEXT(1, &depthBufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, aWidth, aHeight);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FrameBufferId);
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void FrameBufferObject::bind()
{
	// bind the framebuffer, m_FrameBufferId, so operations will now occur on it
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FrameBufferId);
}

/**
 */
void FrameBufferObject::errcheck() const
{
	static GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error: %s\n", errString);
		exit(1);
	}

}

/************************************************************************/
/* STATIC METHODS                                                       */
/************************************************************************/
void FrameBufferObject::unbind()
{
	//'unbind' the frambuffer object, so subsequent drawing ops are not
	// drawn into the FBO. 
	// '0' means "windowing system provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}