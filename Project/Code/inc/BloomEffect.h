#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "LuminanceConverter.h"
#include "GPUParallelReductor.h"
#include "BlurTexture.h"

//const int KNumberOfBlurLevels = 4;
//const int KNumberOfBlurShaders = 1;

class BloomEffect
{
public:
	BloomEffect();
	~BloomEffect();
	void Begin();
	GLuint End();

private:

	enum TTextureSize
	{
		ETextureSize512 = 512,
		ETextureSize256 = 256,
		ETextureSize128 = 128,
		ETextureSize64  = 64,
		ETextureSize32  = 32,
		ETextureSize16  = 16
	};

	enum TMipMapLevel
	{
		EMipMapLevel512 = 0,
		EMipMapLevel256,
		EMipMapLevel128,
		EMipMapLevel64,
		EMipMapLevel32,
		EMipMapLevel16,
		EMipMapLevel8,
		EMipMapLevel4,
		EMipMapLevel2,
		EMipMapLevel1
	};

	enum TTextureID
	{
		ETextureId128 = 0,
		ETextureId64,
		ETextureId32,
		ETextureId16
	};


	void Init();
	void InitTextures();
	void InitFramebufferObject();
	void InitShaders();
	void InitCodecShaderObject();
	void InitBrightpassShader();
	//void InitBlurShaders();
	void InitBlendShader();

	//void BlurMipmap( GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aMipmapLevel );

	void RenderSceneOnQuad( GLuint aTextureID);
	//void RenderSceneOnQuad( GLuint aTextureID, GLuint aVertexSize );
	void RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aBlurredTextures );
	void RenderBloomEffect(GLuint aOriginalTexture, GLuint* aBlurredTextures);

	GLint iOldViewPort[4];

	GLuint iOriginalImageSize;


	FrameBufferObject* iOriginalFBO;
	//FrameBufferObject* iIntermediateFBO[KNumberOfBlurLevels];
	FrameBufferObject* iBlendedFBO;

	GLuint iOriginalTexture;
	GLuint iBrightpassTexture;
	//GLuint iHorizBlurredTexture[KNumberOfBlurLevels];
	//GLuint iFinalBlurredTexture[KNumberOfBlurLevels];
	GLuint iBlendedTexture;

	//GLuint iMipmapSize[KNumberOfBlurLevels];
	//GLuint iMipmapDelta[KNumberOfBlurLevels];

	LuminanceConverter* iLuminanceConverter;
	GPUParallelReductor* iGPUParallelReductor;
	float logSum[4];

	BlurTexture* iBlurTexture;
	GLuint* finalBlurredTextures;

	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iBrightpassShaderProgram;
	//ShaderProgram* iHorizontalShaderProgram;
	//ShaderProgram* iVerticalShaderProgram;
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iBrightpassFragmentShader;
	//ShaderObject* iHorizontalBlurFragmentShader;
	////ShaderObject* iHorizontalBlurVertexShader;
	//ShaderObject* iVerticalBlurFragmentShader;
	ShaderObject* iBlenderFragmentShader;
	ShaderObject* iCodecRGBEFragmentShader;

	ShaderUniformValue<float> iLogAvgLum;

	//ShaderUniformValue<int> iTextureOriginalUniform;
	//ShaderUniformValue<int> iTextureHorizontalUniform;
	//ShaderUniformValue<int> iMipmapSizeUniform;
	//ShaderUniformValue<int> iHorizTextureSizeUniform;
	//ShaderUniformValue<float> iMipmapLevelUniform;

	//ShaderUniformValue<float> iHorizBlurWeight1Uniform;
	//ShaderUniformValue<float> iHorizBlurWeight2Uniform;
	//ShaderUniformValue<float> iHorizBlurWeight3Uniform;
	//ShaderUniformValue<float> iVertBlurWeight1Uniform;
	//ShaderUniformValue<float> iVertBlurWeight2Uniform;
	//ShaderUniformValue<float> iVertBlurWeight3Uniform;

	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;
};