#pragma once

#include "Material.h"

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;


class Diffuse : public Material
{
public:
	Diffuse( const string& ashaderFilename, GLuint aCubeMapTexId );
	~Diffuse(void);

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

	GLuint iCubeMapTexId;
};
