#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"

const int KNumberOfBlurLevels = 4;

class BloomEffect
{
public:
	BloomEffect(unsigned int width, unsigned int height);
	~BloomEffect();
	void Begin();
	void End();

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

	void BlurMipmap( GLuint aTextureID, GLuint aIntermediateTextureNumber, GLuint aMipmapSize, GLuint aMipmapLevel, FrameBufferObject* aFinalFBO );

	void RenderSceneOnQuad( GLuint aTextureID);
	void RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aUpscaledTexture );
	void UpscaleTexture( GLuint aTextureID, GLfloat aTextureCoord );
	void RenderBloomEffect(GLuint aOriginalTexture, GLuint* aUpscaledTexture);



	GLint iOldViewPort[4];

	GLuint iOriginalImageSize;

	//GLuint iImageBlurWidth;
	//GLuint iImageBlurHeight;


	FrameBufferObject* iOriginalFBO;
	FrameBufferObject* iIntermediateFBO[KNumberOfBlurLevels];
	FrameBufferObject* iFinalBlurFBO;

	GLuint iOriginalTexture;
	GLuint iHorizBlurredTexture[KNumberOfBlurLevels];
	GLuint iFinalBlurredTexture[KNumberOfBlurLevels];
	GLuint iUpscaledTexture[KNumberOfBlurLevels];

	GLuint iMipmapSize[KNumberOfBlurLevels];


	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iHorizontalShaderProgram;
	ShaderProgram* iVerticalShaderProgram;
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iBlenderFragmentShader;
	ShaderObject* iHorizontalBlurFragmentShader;
	ShaderObject* iVerticalBlurFragmentShader;

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
	
	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;

	ShaderUniformValue<float> iBlurDeltaUniform;
	ShaderUniformValue<float> iMipmapLevelUniform;
};