#include "CodecRGBE.h"
#include "ShaderObject.h"

CodecRGBE* CodecRGBE::iInstance = NULL;

CodecRGBE::CodecRGBE()
{
	iCodecRGBEFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "./shader/codecRGBE.frag");
}

CodecRGBE::~CodecRGBE()
{
	delete iCodecRGBEFragmentShader;
}

ShaderObject* CodecRGBE::getShaderObject()
{
	if (iInstance == NULL)
	{
		iInstance = new CodecRGBE();
	}

	return iInstance->iCodecRGBEFragmentShader;
}