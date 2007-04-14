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

	void start();

	void stop();

	// methods
private:

	void initShaders(const string& ashaderFilename);

	void setupIrradianceMapTexture(const string& aPanoramaFilename);

	// attributes
private:
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderObject* iVertexShader;
	ShaderUniformValue<int>* iIrradianceMapUniform;

	GLuint iIrradianceMapTexId;
};
