#pragma once

#include "Basic.h"

class Material
{
public:
	virtual void start() = 0;
	virtual void stop() = 0;


	virtual void setEnvironmentCubeMap( GLuint aCubeMapTexId ) {};

	virtual void setDiffuseConvCubeMap( GLuint aCubeMapTexId ) {};
};
