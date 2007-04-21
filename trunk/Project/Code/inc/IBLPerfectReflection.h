#pragma once

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;


class IBLPerfectReflection
{
public:
	IBLPerfectReflection(const string& ashaderFilename, const string& aPanoramaFilename);
	~IBLPerfectReflection(void);

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
};
