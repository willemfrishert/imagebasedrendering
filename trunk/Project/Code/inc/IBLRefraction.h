#pragma once

#include "Material.h"

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;

class IBLRefraction : public Material
{
public:
	IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename, GLuint aCubeMapTexId);
	~IBLRefraction(void);

	void start();

	void stop();

	void setEnvironmentCubeMap( GLuint aCubeMapTexId );


	// methods
private:

	void initShaders(const string& ashaderFilename);

	// attributes
private:
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderObject* iVertexShader;
	ShaderUniformValue<int>* iCubeMapUniform;
	ShaderUniformValue<float>* iEtaUniform;
	ShaderUniformValue<float>* iFresnelPowerUniform;
	ShaderUniformValue<float>* iFUniform;

	float iEta1;
	float iEta2;

	GLuint iCubeMapTexId;
};
