#pragma once

// forward declarations
class FrameBufferObject;
class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;

class GPUParallelReductor
{
public:
	GPUParallelReductor(float* values, int width, int height, string fragmentShaderFilename, 
		GLint texInternalFormat, GLenum texFormat = GL_LUMINANCE);
	
	GPUParallelReductor(int width, int height, string fragmentShaderFilename, 
		GLint texInternalFormat, GLenum texFormat = GL_LUMINANCE);
	
	~GPUParallelReductor(void);

	void processData(float* result);

	void processData(GLuint originalTexId, float* result);

	void setOriginalTexture(GLuint originalTexId);

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
	ShaderUniformValue<float>* iTexHeightUniform;
};


