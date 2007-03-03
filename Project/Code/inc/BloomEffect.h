#pragma once

#include "Basic.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "ShaderObject.h"
#include "ShaderUniformValue.h"

const int KNumberOfBlurMaps = 3;
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

	void BlurMipmap(GLuint aTextureID,
					GLuint aMipmapSize,
					GLenum aDestinationMode,
					GLuint aDestinationWidth,
					GLuint aDestinationHeight);

	void RenderSceneOnQuad( GLuint aTextureID);
	void UpscaleTexture( GLuint aTextureID, GLfloat aTextureCoord );

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
	ShaderUniformValue<int> iBlenderTextureUniform;
	ShaderUniformValue<float> iBlurDeltaUniform;
	ShaderUniformValue<float> iMipmapLevelUniform;

};