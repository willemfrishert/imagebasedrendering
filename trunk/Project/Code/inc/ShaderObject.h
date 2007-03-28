/**
 * @description Represents a GLSL shader (vertex or fragment).
 * 
 * @file ShaderObject.h
 * @author João Pedro Jorge
 */
#pragma once
#include "Basic.h"


class ShaderObject
{
public:
	ShaderObject(GLenum shaderType, string filename);
	ShaderObject(GLenum shaderType, char* filename);
	~ShaderObject(void);

	GLuint getShaderId() const;

private:
	unsigned char* readShaderFile(const char *filename);
	void init(const char* filename, GLenum shaderType);

	// attributes
private:
	const char *shaderStrings[1];
	GLuint shaderId;
};
