#include "Basic.h"

class ShaderObject;

class CodecRGBE
{
public:
	static ShaderObject* getShaderObject();
private:
	CodecRGBE();
	~CodecRGBE();

	ShaderObject* iCodecRGBEFragmentShader;

	static CodecRGBE* iInstance;
};


