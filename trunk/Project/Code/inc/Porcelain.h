#pragma once

#include "Material.h"

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;


class Porcelain : public Material
{
public:
	Porcelain(float aEta1, float aEta2,const string& ashaderFilename, GLuint aCubeMapTexId, GLuint aDiffuseCubeMapTexId);
	~Porcelain(void);

	void start();

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
	ShaderUniformValue<int>* iDiffuseCubeMapUniform;
	ShaderUniformValue<float>* iMaterialReflectance;
	ShaderUniformValue<float>* iFresnelPower;

	float iEta1;
	float iEta2;

	GLuint iCubeMapTexId;
	GLuint iDiffuseCubeMapTexId;
};
