#pragma once

class FrameBufferObject;
class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;
template <class T>
class ShaderUniformVector;

class LuminanceConverter
{
public:
	LuminanceConverter(int width, int height, int minExponent, int maxExponent);
	~LuminanceConverter(void);

	GLuint processData(GLuint inputTextureId);

	// methods
private:

	void setupTexture(GLuint textureId, const GLvoid* data);

	void initShaders();

	void renderSceneOnQuad(GLuint textureId, GLenum target);

	// attributes
private:

	int iHeight;
	int iWidth;
	int iMinExponent;
	int iMaxExponent;
	FrameBufferObject* iFbo;

	GLuint iLuminanceTexture;

	// shader stuff
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderUniformValue<int>* iInputTextureUniform;
	ShaderUniformVector<float>* iScaleBias;
};
