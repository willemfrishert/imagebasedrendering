/**
 * @usage: Create object; bind and attach texture
 * 
 * @file FrameBufferObject.h
 * @author João Pedro Jorge
 */
class FrameBufferObject
{
public:
	FrameBufferObject(void);
	~FrameBufferObject(void);

	void attachTexture(GLuint texture, GLenum attachment, GLenum textarget, GLint mipmapLevel);
	void attachDepthBuffer(GLuint depthBufferId);

	void bind();
	static void unbind();

	// methods
private:
	void errcheck() const;

	// attributes
private:
	GLuint m_FrameBufferId;

};