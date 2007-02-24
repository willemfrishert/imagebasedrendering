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
	FloatTextProcessor(float* values, int width, int height);
	~FloatTextProcessor(void);

	void processData();

	// methods
private:

	void renderSceneOnQuad(GLuint textureId, GLenum target, int width, int height);
	// attributes
private:

	int iHeight;
	int iWidth;
	float* iValues;
	FrameBufferObject* iFbo;
	
	GLuint iTextures[2];

	// shader stuff
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderUniformValue<int>* iInputTextureUniform;
};