#include "OpenGLUtility.h"

void OpenGLUtility::RenderSceneOnQuadMultiTex(const vector<GLuint>& aTextureId,
											  const vector<GLenum>& aTextureUnit)
{
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
			EnableMultitexturing(aTextureId, aTextureUnit);

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

	DisableMultitexturing( aTextureUnit );
}

void OpenGLUtility::EnableMultitexturing( const vector<GLuint>& aTextureId, const vector<GLenum>& aTextureUnit )
{
	vector<GLuint>::const_iterator texIterator = aTextureId.begin();
	vector<GLenum>::const_iterator unitIterator = aTextureUnit.begin();

	for (;(texIterator!=aTextureId.end()) && (unitIterator!= aTextureUnit.end()); texIterator++, unitIterator++)
	{

		GLenum i = *unitIterator;
		// enable multitexturing
		glActiveTexture( i );
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *texIterator );
	}
}

void OpenGLUtility::DisableMultitexturing( const vector<GLenum>& aTextureUnit )
{
	vector<GLenum>::const_iterator unitIterator = aTextureUnit.begin();

	for (;unitIterator!= aTextureUnit.end(); unitIterator++)
	{
		glActiveTexture( *unitIterator );
		glDisable(GL_TEXTURE_2D);
	}

	// GL_TEXTURE0: Original Texture
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
}

