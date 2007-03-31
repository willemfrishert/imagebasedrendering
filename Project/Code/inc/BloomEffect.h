#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"

const int KNumberOfBlurMaps = 4;
class BloomEffect
{
public:
	BloomEffect(unsigned int width, unsigned int height);
	~BloomEffect();
	void Begin();
	void End();

private:
	void Init();
	void InitTextures();
	void InitFramebufferObject();
	void InitShaders();

	void BlurMipmap( GLuint aTextureID,  GLuint aMipmapSize,  GLfloat aMipmapLevel, GLfloat aUpscaleFactor, FrameBufferObject* aFinalFBO );

	void RenderSceneOnQuad( GLuint aTextureID);
	void RenderSceneOnQuad(  GLuint aOriginalTexture, GLuint* aUpscaledTexture );
	void UpscaleTexture( GLuint aTextureID, GLfloat aTextureCoord );
	void RenderBloomEffect(GLuint aOriginalTexture, GLuint* aUpscaledTexture);

	FrameBufferObject* iOriginalFBO;
	FrameBufferObject* iIntermediateFBO;
	FrameBufferObject* iFinalBlurFBO;

	GLint iOldViewPort[4];

	GLuint iImageWinWidth;
	GLuint iImageWinHeight;

	GLuint iImageBlurWidth;
	GLuint iImageBlurHeight;

	GLuint iOriginalTexture;
	GLuint iHorizBlurredTexture, iFinalBlurredTexture;
	GLuint iUpscaledTexture[KNumberOfBlurMaps];


	// ########### SHADERS DECLARATIONS ###########
	ShaderProgram* iHorizontalShaderProgram;
	ShaderProgram* iVerticalShaderProgram;
	ShaderProgram* iBlenderShaderProgram;

	ShaderObject* iBlenderVertexShader;
	ShaderObject* iBlenderFragmentShader;

	ShaderObject* iHorizontalBlurVertexShader;
	ShaderObject* iHorizontalBlurFragmentShader;

	ShaderObject* iVerticalBlurVertexShader;
	ShaderObject* iVerticalBlurFragmentShader;

	ShaderUniformValue<int> iTextureOriginalUniform;
	ShaderUniformValue<int> iTextureHorizontalUniform;
	ShaderUniformValue<int> iMipmapSizeUniform;
	ShaderUniformValue<int> iHorizTextureSizeUniform;
	
	ShaderUniformValue<int> iBlenderOriginalTextureUniform;
	ShaderUniformValue<int> iBlenderBlur1TextureUniform;
	ShaderUniformValue<int> iBlenderBlur2TextureUniform;
	ShaderUniformValue<int> iBlenderBlur3TextureUniform;
	ShaderUniformValue<int> iBlenderBlur4TextureUniform;

	ShaderUniformValue<float> iBlurDeltaUniform;
	ShaderUniformValue<float> iMipmapLevelUniform;

};