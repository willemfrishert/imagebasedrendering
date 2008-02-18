#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "LuminanceConverter.h"
#include "GPUParallelReductor.h"
#include "BlurTexture.h"
#include "OpenGLUtility.h"
#include "IKeyListener.h"

class BloomEffect : public IKeyListener
{
public:
	BloomEffect(int aTextureSize);
	~BloomEffect();
	void Begin();
	GLuint End();
	GLuint ApplyEffect(GLuint aOriginalTexture, float* logSum);

	// Key handling events
	virtual void ProcessNormalKeys( unsigned char key, int x, int y );
	virtual void ProcessCursorKeys( int key, int x, int y );

private:

	void Init();
	void InitTextures();
	void InitFramebufferObject();
	void InitShaders();
	void InitBrightpassShader();
	void InitBlendShader();

	void RenderBloomEffect(const GLuint* iTextureLayers);
	
	GLint iOldViewPort[4];

	GLuint iOriginalImageSize;

	FrameBufferObject* iOriginalFBO;
	FrameBufferObject* iBlendedFBO;

	GLuint iOriginalTexture;
	GLuint iBrightpassTexture;
	GLuint iBlendedTexture;

	float logSum[4];

	BlurTexture* iBlurTexture;
	GLuint* iTextureLayers;

	int iCycleTextureOutput;

	vector<GLuint> iOriginalTexIdVector;

	// ########### SHADERS DECLARATIONS ##########
	ShaderProgram* iBrightpassShaderProgram;
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iBrightpassFragmentShader;
	ShaderObject* iBlenderFragmentShader;
	//ShaderObject* iCodecRGBEFragmentShader;

	ShaderUniformValue<float> iLogAvgLum;

	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;
};


