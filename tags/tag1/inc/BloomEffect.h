#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"

const int KNumberOfBlurLevels = 4;
const int KNumberOfBlurShaders = 1;

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
	void InitBlurShaders();
	void InitBlendShader();

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
	FrameBufferObject* iUpscaleBlurFBO;
	FrameBufferObject* iBlendedFBO;

	GLuint iOriginalTexture;
	GLuint iHorizBlurredTexture[KNumberOfBlurLevels];
	GLuint iFinalBlurredTexture[KNumberOfBlurLevels];
	GLuint iUpscaledTexture[KNumberOfBlurLevels];
	GLuint iBlendedTexture;

	GLuint iMipmapSize[KNumberOfBlurLevels];


	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iHorizontalShaderProgram[KNumberOfBlurShaders];
	ShaderProgram* iVerticalShaderProgram[KNumberOfBlurShaders];
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iHorizontalBlurFragmentShader[KNumberOfBlurShaders];
	ShaderObject* iVerticalBlurFragmentShader[KNumberOfBlurShaders];
	ShaderObject* iBlenderFragmentShader;

	ShaderUniformValue<int> iTextureOriginalUniform[KNumberOfBlurShaders];
	ShaderUniformValue<int> iTextureHorizontalUniform[KNumberOfBlurShaders];
	ShaderUniformValue<int> iMipmapSizeUniform[KNumberOfBlurShaders];
	ShaderUniformValue<int> iHorizTextureSizeUniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iMipmapLevelUniform[KNumberOfBlurShaders];

	ShaderUniformValue<float> iHorizBlurWeight1Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iHorizBlurWeight2Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iHorizBlurWeight3Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iHorizBlurWeight4Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iHorizBlurWeight5Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iVertBlurWeight1Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iVertBlurWeight2Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iVertBlurWeight3Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iVertBlurWeight4Uniform[KNumberOfBlurShaders];
	ShaderUniformValue<float> iVertBlurWeight5Uniform[KNumberOfBlurShaders];
	
	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;

	ShaderObject* iCodecRGBEFragmentShader;
};