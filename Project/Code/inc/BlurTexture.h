#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"

const int KNumberOfBlurLevels = 4;
const int KNumberOfBlurShaders = 1;

class BlurTexture
{
public:
	BlurTexture();
	~BlurTexture();
	GLuint* BlurTexture::processData(GLuint aTextureId);

private:

	enum TTextureSize
	{
		ETextureSize512 = 512,
		ETextureSize256 = 256,
		ETextureSize128 = 128,
		ETextureSize64  = 64,
		ETextureSize32  = 32,
		ETextureSize16  = 16,
		ETextureSize8   = 8,
		ETextureSize4   = 4,
		ETextureSize2   = 2,
		ETextureSize1   = 1
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
	void InitBlurShaders();
	
	void BlurMipmap( GLuint aTextureID, GLuint aCounter, GLuint aMipmapSize, GLuint aBlurPasses );

	void RenderSceneOnQuad( GLuint aTextureID);
	void RenderSceneOnQuad( GLuint aTextureID, GLuint aVertexSize );

	GLint iOldViewPort[4];

	GLuint iOriginalImageSize;


	FrameBufferObject* iOriginalFBO;
	FrameBufferObject* iIntermediateFBO[KNumberOfBlurLevels];
	FrameBufferObject* iBlendedFBO;

	GLuint iOriginalTexture;
	GLuint iHorizBlurredTexture[KNumberOfBlurLevels];
	GLuint iFinalBlurredTexture[KNumberOfBlurLevels];
	GLuint iDownScaledTexture[KNumberOfBlurLevels];

	GLuint iMipmapSize[KNumberOfBlurLevels];
	GLuint iMipmapDelta[KNumberOfBlurLevels];

	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iHorizontalShaderProgram;
	ShaderProgram* iVerticalShaderProgram;

	ShaderObject* iHorizontalBlurFragmentShader;
	ShaderObject* iVerticalBlurFragmentShader;
	ShaderObject* iCodecRGBEFragmentShader;

	ShaderUniformValue<int> iTextureOriginalUniform;
	ShaderUniformValue<int> iTextureHorizontalUniform;
	ShaderUniformValue<int> iMipmapSizeUniform;
	ShaderUniformValue<int> iHorizTextureSizeUniform;

	ShaderUniformValue<float> iHorizBlurWeight1Uniform;
	ShaderUniformValue<float> iHorizBlurWeight2Uniform;
	ShaderUniformValue<float> iHorizBlurWeight3Uniform;
	ShaderUniformValue<float> iVertBlurWeight1Uniform;
	ShaderUniformValue<float> iVertBlurWeight2Uniform;
	ShaderUniformValue<float> iVertBlurWeight3Uniform;
};