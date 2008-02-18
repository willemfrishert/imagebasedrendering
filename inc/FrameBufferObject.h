#pragma once
#include "Basic.h"

/**
 * @usage: Create object; bind and attach texture
 * 
 * @file FrameBufferObject.h
 * @author João Pedro Jorge
 */

struct FBOTextureAttachment
{
	GLuint id;
	GLenum attachment;
	GLenum target;
	GLint mipmapLevel;
};

class FrameBufferObject
{
public:
	FrameBufferObject(void);
	~FrameBufferObject(void);

	void attachTexture(GLuint texture, GLenum attachment, GLenum textarget, GLint mipmapLevel);
	void attachTextureAttachment(const FBOTextureAttachment& attachment);
	void attachDepthBuffer(GLuint depthBufferId);
	void attachDepthRenderBuffer(GLuint aWidth, GLuint aHeight);

	void bind();
	static void unbind();

	// methods
private:
	void errcheck() const;

	// attributes
private:
	GLuint m_FrameBufferId;

};



