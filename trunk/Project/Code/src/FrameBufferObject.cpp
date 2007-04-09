#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <assert.h>
#include "FrameBufferObject.h"

#define CHECK_FRAMEBUFFER_STATUS() \
{\
	GLenum status; \
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
	switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
 /*fprintf(stderr,"framebuffer complete!\n");*/\
 break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
 fprintf(stderr,"GL_FRAMEBUFFER_UNSUPPORTED_EXT: The combination of internal formats of the attached \
images does violate an implementation-dependent set of \
restrictions\n");\
 /* you gotta choose different formats */ \
 assert(0); \
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
 fprintf(stderr,"INCOMPLETE_ATTACHMENT: Not all framebuffer attachment points are \"framebuffer \
attachment complete\"\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
 fprintf(stderr,"FRAMEBUFFER_MISSING_ATTACHMENT: There should be at least one image \
attached to the framebuffer.\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
 fprintf(stderr,"FRAMEBUFFER_DIMENSIONS: All attached images have the same width and height\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
 fprintf(stderr,"INCOMPLETE_FORMATS: All images attached to the attachment points\
COLOR_ATTACHMENT0_EXT through COLOR_ATTACHMENTn_EXT must have\
the same internal format.\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
 fprintf(stderr,"INCOMPLETE_DRAW_BUFFER: The value of FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT \
must not be NONE for any color attachment point(s) named by DRAW_BUFFERi\n");\
 break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
 fprintf(stderr,"INCOMPLETE_READ_BUFFER: If READ_BUFFER is not NONE, then the value of\
FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT must not be NONE for the color attachment\
 point named by READ_BUFFER\n");\
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

#ifdef _DEBUG
	errcheck();
#endif // _DEBUG
}

FrameBufferObject::~FrameBufferObject(void)
{
}

/**
 * @param texture
 * @param attachment
 */
void FrameBufferObject::attachTexture( GLuint texture, GLenum attachment, 
									  GLenum textarget, GLint mipmapLevel )
{
	// bind this texture to the current framebuffer obj. as 
	// attachment 
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
		attachment, textarget, texture, mipmapLevel);

#ifdef _DEBUG
	errcheck();

	//see if everything is OK
	CHECK_FRAMEBUFFER_STATUS()
#endif // _DEBUG
}

void FrameBufferObject::attachDepthBuffer(GLuint depthBufferId)
{
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
		GL_DEPTH_ATTACHMENT_EXT,
		GL_RENDERBUFFER_EXT, depthBufferId);

#ifdef _DEBUG
	errcheck();

	//see if everything is OK
	CHECK_FRAMEBUFFER_STATUS()
#endif // _DEBUG
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