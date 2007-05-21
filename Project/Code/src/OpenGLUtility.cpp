#include "OpenGLUtility.h"

void OpenGLUtility::RenderSceneOnQuadMultiTex(const GLuint* aTextureId, int aNumberOfTextures)
{
	static GLenum aTextureUnit[] = { GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4,
									 GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9 };

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	{
		glLoadIdentity();
		gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f);

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		{
			glLoadIdentity();

			// setup flags and bind textures
			EnableMultitexturing(aTextureId, aTextureUnit, aNumberOfTextures);

			glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f );
			glVertex2f(0.0, 0.0);

			glTexCoord2f(1.0f, 0.0f );
			glVertex2f(1.0f, 0.0);

			glTexCoord2f(1.0f, 1.0f );
			glVertex2f(1.0f, 1.0f);

			glTexCoord2f(0.0f, 1.0f );
			glVertex2f(0.0, 1.0f);

			glEnd();
		} //End GL_PROJECTION

		glPopMatrix();

		glMatrixMode( GL_PROJECTION );
	}
	glPopMatrix(); // End GL_PROJECTION

	glMatrixMode( GL_MODELVIEW );

	DisableMultitexturing( aTextureUnit, aNumberOfTextures );
}

void OpenGLUtility::EnableMultitexturing( const GLuint* aTextureId, const GLenum* aTextureUnit, int aNumberOfTextures )
{
	for (int i = 0; i < aNumberOfTextures; i++)
	{
		// enable multitexturing
		glActiveTexture( aTextureUnit[i] );
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, aTextureId[i] );
	}
}

void OpenGLUtility::DisableMultitexturing( const GLenum* aTextureUnit, int aNumberOfTextures )
{
	for (int i = 0; i < aNumberOfTextures; i++)
	{
		glActiveTexture( aTextureUnit[i] );
		glDisable(GL_TEXTURE_2D);
	}

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
}

