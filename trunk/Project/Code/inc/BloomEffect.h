#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "LuminanceConverter.h"
#include "GPUParallelReductor.h"
#include "BlurTexture.h"

class BloomEffect
{
public:
	BloomEffect();
	~BloomEffect();
	void Begin();
	GLuint End();

private:

	void Init();
	void InitTextures();
	void InitFramebufferObject();
	void InitShaders();
	void InitCodecShaderObject();
	void InitBrightpassShader();
	void InitBlendShader();

	void RenderSceneOnQuad( GLuint aTextureID);
	void RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aBlurredTextures );
	void RenderBloomEffect(GLuint aOriginalTexture, GLuint* aBlurredTextures);
	void EnableMultitexturing( GLuint aOriginalTexture,  GLuint aLuminanceTexture );
	void DisableMultitexturing();
	void RenderSceneOnQuadMultiTex(GLuint aOriginalTexture, GLuint aLuminanceTexture);

	GLint iOldViewPort[4];

	GLuint iOriginalImageSize;


	FrameBufferObject* iOriginalFBO;
	FrameBufferObject* iBlendedFBO;

	GLuint iOriginalTexture;
	GLuint iBrightpassTexture[2];
	GLuint iBlendedTexture;

	unsigned int iBrightpassToggle;
	GLenum iBrightpassFBOAttachment[2];

	LuminanceConverter* iLuminanceConverter;
	GPUParallelReductor* iGPUParallelReductor;
	float logSum[4];

	BlurTexture* iBlurTexture;
	GLuint* iFinalBlurredTexture;

	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iBrightpassShaderProgram;
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iBrightpassFragmentShader;
	ShaderObject* iBlenderFragmentShader;
	ShaderObject* iCodecRGBEFragmentShader;

	ShaderUniformValue<float> iLogAvgLum;

	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;
	ShaderUniformValue<int> iPrevBrightpassTextureUniform;
};