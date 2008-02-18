#pragma once

// forward declarations
class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;
class GPUParallelReductor;

class SShapedCurveToneMapper
{
public:
	SShapedCurveToneMapper(GLuint aOriginalTexture, GLuint aLuminanceTexture, int aWidth, int aHeight);
	~SShapedCurveToneMapper(void);

	void toneMap(GLuint aOriginalTexture, GLuint aLuminanceTexture);

	void setExposure(float aValue);

	float getExposure();

	void InvalidateExposure();

	// methods
private:

	void enableMultitexturing(GLuint aOriginalTexture,  GLuint aLuminanceTexture);

	void disableMultitexturing();

	void renderSceneOnQuad(GLuint aOriginalTexture, GLuint aLuminanceTexture);

	void setupTexture(GLuint textureId);

	void initShaders( string fragmentShaderFilename );

	float computeCurrentExposure(float aLogLAverage, float aLogLMin, float aLogLMax);

	// attributes
private:

	GLuint iOriginalTexture;
	GLuint iLuminanceTexture;
	int iWidth;
	int iHeight;

	// shader stuff
	ShaderProgram* iShaderProgram;
	ShaderObject* iFragmentShader;
	ShaderUniformValue<int>* iOriginalTextureUniform;
	ShaderUniformValue<int>* iLuminanceTextureUniform;
	ShaderUniformValue<float>* iLogAverageUniform;
	ShaderUniformValue<float>* iExposureUniform;
	ShaderUniformValue<float>* iSensitivityUniform;

	// Parallel reduction
	GPUParallelReductor* iLogAverageCalculator;
	
	// Exposure settings
	float iPreviousExposure;
	bool iInvalidateExposure;
};
