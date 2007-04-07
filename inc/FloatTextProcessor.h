#pragma once

// forward declarations
class FrameBufferObject;
class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;

class FloatTextProcessor
{
public:
	FloatTextProcessor(float* values, int width, int height, string fragmentShaderFilename, 
		GLint texInternalFormat = GL_RGB_FLOAT32_ATI, GLenum texFormat = GL_LUMINANCE);
	
	FloatTextProcessor(GLuint originalTexId, int width, int height, string fragmentShaderFilename, 
		GLint texInternalFormat = GL_RGB_FLOAT32_ATI, GLenum texFormat = GL_LUMINANCE);
	~FloatTextProcessor(void);

	float processData();

	// methods
private:

	void renderSceneOnQuad(GLuint textureId, GLenum target, int width, int height);

	void generateTextures(float* originalData);

	void setupTexture(GLuint textureId, const GLvoid* data);

	void initShaders(string fragmentShaderFilename);

	// attributes
private:

	int iHeight;
	int iWidth;
	FrameBufferObject* iFbo;
	
	GLuint iTextures[2];
	GLuint iOriginalTexture;

	GLint internalFormat;
	GLenum format;

	// shader stuff
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderObject* iVertexShader;
	ShaderUniformValue<int>* iInputTextureUniform;
};