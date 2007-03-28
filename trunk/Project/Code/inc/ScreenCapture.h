#pragma once

typedef unsigned int GLuint;
typedef void GLvoid;
class FrameBufferObject;

class ScreenCapture
{
public:
	ScreenCapture(int width, int height);
	~ScreenCapture(void);

	void changeResolution(int width, int height);

	void startCapture();
	GLuint stopCapture();

	void renderToScreen();

	// methods
private:

	void setupTexture(GLuint textureId, const GLvoid* data, int width, int height);
	void renderSceneOnQuad(GLuint textureId, GLenum target);

	// attributes
private:

	int iWidth;
	int iHeight;
	FrameBufferObject* iCaptureFBO;
	GLuint iCaptureTexture;
};
