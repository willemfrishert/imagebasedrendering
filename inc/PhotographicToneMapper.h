#pragma once

// forward declarations
class ShaderObject;
class ShaderProgram;
template <class T>
class ShaderUniformValue;
class GPUParallelReductor;

class PhotographicToneMapper
{
public:
	PhotographicToneMapper(GLuint aOriginalTexture, GLuint aLuminanceTexture, int aWidth, int aHeight);
	~PhotographicToneMapper(void);

	void ToneMap(GLuint aOriginalTexture, GLuint aLuminanceTexture, float* logStatistics);

	void setExposure(float aValue);

	float getExposure();

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
	ShaderUniformValue<float>* iExposureUniform;

	// Parallel reduction
	GPUParallelReductor* iLogAverageCalculator;
	
	// Exposure settings
	float iPreviousExposure;
	float iBaseExposure;
};
