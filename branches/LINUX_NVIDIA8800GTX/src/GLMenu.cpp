#include "GLMenu.h"

GLMenu::GLMenu(int aWindowHeight, int aWindowWidth, int aGlutFont)
: iWindowHeight( aWindowHeight )
, iWindowWidth( aWindowWidth )
, iGlutFont( aGlutFont )
{
}

GLMenu::~GLMenu(void)
{
	std::vector<char*>::iterator linesIt = lines.begin();
	std::vector<char*>::iterator end = lines.end();

	for(; linesIt != end; ++linesIt)
	{
		//delete (*linesIt);
	}
}

void GLMenu::addMenuLine(string line)
{
	const char* original = line.c_str();
	char* str = new char[ strlen(original) ];
	strcpy(str, original);
	
	this->lines.push_back( str );
}

void GLMenu::draw()
{
	float startPos = 16;
	const float pixelsPerLine = 48.0;
	static float pixelsizeH = 1.0/iWindowHeight;
	static float indent = (float)iWindowWidth * 0.05f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	{
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glLoadIdentity();

			gluOrtho2D(0, iWindowWidth, 0, iWindowHeight);

			glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);

			glDisable( GL_LIGHTING );
			glDisable( GL_TEXTURE_2D );
			glDisable( GL_DEPTH_TEST );

			std::vector<char*>::const_iterator linesIt = lines.begin();
			std::vector<char*>::const_iterator end = lines.end();
			
			glColor3f(0.5, 0.5, 0.7);
			
			for(; linesIt != end; ++linesIt, startPos+= 16)
			{
				//glRasterPos2f(indent, iWindowHeight - startPos);
				glRasterPos2f(indent, startPos);
				int len = strlen( *linesIt );
				for (unsigned int i = 0; i < len; i++){
					glutBitmapCharacter((void*)iGlutFont,(*linesIt)[i]);
				}   
			}

			glPopAttrib();
		}
		glPopMatrix();
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}