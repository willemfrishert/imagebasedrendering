#include <gl/glew.h>
#include <windows.h>
#include "ShaderObject.h"
#include "ShaderUniformValue.h"
#include "ShaderProgram.h"
#include "CodecRGBE.h"
#include "IKeyListener.h"
#include "IBLRefraction.h"

IBLRefraction::IBLRefraction(float aEta1, float aEta2, const string& ashaderFilename, GLuint aCubeMapTexId)
: iEta1( aEta1 )
, iEta2( aEta2 )
, iCubeMapTexId( aCubeMapTexId )
{
	initShaders( ashaderFilename );
}

IBLRefraction::~IBLRefraction(void)
{
	delete iShaderProgram;
	delete iCubeMapUniform;

	delete iFragmentShader;
	delete iVertexShader;
}

/**
*
*/
void IBLRefraction::start()
{
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, iCubeMapTexId);

	iShaderProgram->useProgram();
}

/**
*
*/
void IBLRefraction::stop()
{
	iShaderProgram->disableProgram();

	glDisable(GL_TEXTURE_CUBE_MAP);
}

void IBLRefraction::setEnvironmentCubeMap( GLuint aCubeMapTexId )
{
	this->iCubeMapTexId = aCubeMapTexId;
}

/**
 * @param ashaderFilename
 */
void IBLRefraction::initShaders(const string& ashaderFilename)
{
	//float eta				= min(iEta2, iEta1) / max(iEta2, iEta1);
	//float f					= ((1.0 - eta) * (1.0 - eta)) / ((1.0 + eta) * (1.0 + eta));

	float f, eta;					
	computeEta(eta, f);

	iShaderProgram			= new ShaderProgram();
	iCubeMapUniform			= new ShaderUniformValue<int>("reflectionCubeMap", 0);
	iEtaUniform				= new ShaderUniformValue<float>("eta", eta);
	iFUniform				= new ShaderUniformValue<float>("f", f);
	iFresnelPowerUniform	= new ShaderUniformValue<float>("fresnelPower", 15.0);

	iFragmentShader			= new ShaderObject(GL_FRAGMENT_SHADER, ashaderFilename + ".frag");
	iVertexShader			= new ShaderObject(GL_VERTEX_SHADER, ashaderFilename + ".vert");

	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *CodecRGBE::getShaderObject() );
	iShaderProgram->addUniformObject( iCubeMapUniform );
	iShaderProgram->addUniformObject( iEtaUniform );
	iShaderProgram->addUniformObject( iFresnelPowerUniform );
	iShaderProgram->addUniformObject( iFUniform );

	// after all the shaders have been attached
	iShaderProgram->buildProgram();
}

void IBLRefraction::computeEta( float& eta, float& f )
{
	eta = min(iEta2, iEta1) / max(iEta2, iEta1);

	f	= ((1.0 - eta) * (1.0 - eta)) / ((1.0 + eta) * (1.0 + eta));
}

/**
 * @param key
 * @param x
 * @param y
 */
void IBLRefraction::ProcessCursorKeys(int key, int x, int y )
{
	
}

/**
 * @brief processes the ordinary keys
 * @param key
 * @param x
 * @param y
 */
void IBLRefraction::ProcessNormalKeys(unsigned char key, int x, int y )
{
	float step = 0.1f;
	float upperBound = 2.5f;
	float lowerBound = 1.0f;
	float eta, f;

	switch( key )
	{
	case 'f':
		{
			float inc = iEta1 + step;
			iEta1 = inc > upperBound ? upperBound: inc;
			computeEta(eta, f);
			iEtaUniform->setValue( eta );
			iFUniform->setValue( f );
		}
		break;
	case 'F':
		{
			float dec = iEta1 - step;
			iEta1 = dec < lowerBound ? lowerBound: dec;
			computeEta(eta, f);
			iEtaUniform->setValue( eta );
			iFUniform->setValue( f );
		}
		break;
	}
}