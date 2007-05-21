#pragma once

#include "Material.h"

class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;
class IKeyListener;

class IBLRefraction : public Material, public IKeyListener
{
public:
	IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename, GLuint aCubeMapTexId);
	~IBLRefraction(void);

	void start();

	void stop();

	void setEnvironmentCubeMap( GLuint aCubeMapTexId );

	virtual void ProcessNormalKeys( unsigned char key, int x, int y );
	
	virtual void ProcessCursorKeys( int key, int x, int y );


	// methods
private:

	void initShaders(const string& ashaderFilename);

	void computeEta(float& eta, float& f);

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
