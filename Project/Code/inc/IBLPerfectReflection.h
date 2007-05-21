#pragma once

#include "Material.h"

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;


class IBLPerfectReflection : public Material
{
public:
	IBLPerfectReflection( const string& ashaderFilename, GLuint aCubeMapTexId );
	~IBLPerfectReflection(void);

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
	GLuint iCubeMapTexId;
};
