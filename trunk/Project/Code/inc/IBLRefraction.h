#pragma once

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;

class IBLRefraction
{
public:
	IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename);
	~IBLRefraction(void);

	void start(GLuint aCubeMapTexId);

	void stop();

	// methods
private:

	void initShaders(const string& ashaderFilename);

	// attributes
private:
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderObject* iVertexShader;
	ShaderUniformValue<int>* iCubeMapUniform;
	ShaderUniformValue<float>* iEta1Uniform;
	ShaderUniformValue<float>* iEta2Uniform;

	float iEta1;
	float iEta2;
};