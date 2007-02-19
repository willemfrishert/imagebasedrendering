//INCLUDES
#include "Renderer.h"
#include "Vertex.h"

//USE THIS FILE


//CONSTANTS
const int KTextureWidth = 512; ///< Texture max size
const int KTextureHeight= 512; ///< Texture max size

//Aspect ratio common values: 4/3(TV), 1.66(Cartoons), 16/9(HDTV), 2.35(Epic Movies)
const float KPerspectiveAspectRatio = float(4/3);
const float KPerspectiveAngle = float(45);

//VIEW FRUSTUM
const float KZNear	= 10.0f;
const float KZFar	= 600.0f;

//INIT STATIC DATA
Renderer* Renderer::iCurrentRenderer = 0;

//CONSTRUCTORS
//
//Default constructor
Renderer::Renderer()
: iFrame(0)
, iPreviousFrame(0)
, iPreviousTime(0)
, iXRotation( 0 )
, iYRotation( 0 )
, iOldXRotation( 0 )
, iOldYRotation( 0 )
, iXPosition( 0 )
, iYPosition( 0 )
, iZoom( -25 )
{
	InitMain();
	Renderer::iCurrentRenderer = this;
}


//
//DESTRUCTOR releases the used heap
//
Renderer::~Renderer()
{
}

void Renderer::InitShaders()
{
}

void Renderer::LoadTextures()
{

}

//
/// Init function for the constructors
void Renderer::InitMain()
{
	//Init view
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE); // Enable the back face culling
	glEnable(GL_DEPTH_TEST); // Enable the depth test (z-buffer)

}

/** \brief Method that creates a mesh
 *
 *   Creates an instance of a desired mesh type
 */
void Renderer::CreateScene()
{
}


/** \brief Method that calculates iFrames per second
*
*	Method that uses glut to calculate iFrames per second. It update the iFpsCountString every  1000 calculation.
*	This method is to uncertain to do any scientific conclusiions but give you a chanse to evaluate the
*	speed of you code.<br>
*	The more math you implement the slower the iFrame rate should be, but not to slowe.
*
*/
void Renderer::FramesPerSec()
{
	iFrame++;
	iCurrentTime = glutGet(GLUT_ELAPSED_TIME);

	if ( iCurrentTime - iPreviousTime > 1000 )
	{
		//secure sprintf_s: (required by visualstudio 2005)
#ifdef _WIN32
		//sprintf_s(iFpsCountString,"FPS:%4.2f", iFrame*1000.0/( iCurrentTime - iPreviousTime ));
		sprintf_s  (iFpsCountString,"FPS:%4.2f", iFrame*1000.0/( iCurrentTime - iPreviousTime ));
#else
		sprintf(iFpsCountString,"FPS:%4.2f", iFrame*1000.0/( iCurrentTime - iPreviousTime ) );
#endif
		iPreviousTime = iCurrentTime;
		iFrame = 0;
	}

}

/** \brief Method that draws text to the glut window
*
*	This method draws text to the glut window at the postion x = -0.85 and y = -0.95
*	If you want to print you own text, you might want to change the method to something like
*	DrawText(int x, int y, cons char *string)
*
*/
void Renderer::DrawText() const
{
	static GLfloat quadratic[3] = { 1.0f, 0.0f, 0.0f };
	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, quadratic );

	float x( -0.9f ), y( -0.9f );
	
	//configure the transforms for 2D
	glMatrixMode( GL_PROJECTION );

	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( -1.0f, 1.0f, -1.0f, 1.0f);

	glDisable(GL_LIGHTING);
	glDisable( GL_TEXTURE_2D );
	glDisable(GL_DEPTH_TEST);

	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(x, y);

	for (int i = 0, len = static_cast<int>( strlen(iFpsCountString) ); i < len; i++)
	{
		//glutBitmapCharacter(GLUT_BITMAP_9_BY_15, iFpsCountString[i] );
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, iFpsCountString[i] );
	}
	//glFlush();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


// ********** GL DRAWING METHODs *******


// *************	RENDERING METHODS *********** /


void Renderer::InitLights()
{

}

/** 
 *\brief Method that specifies how the screen is rendered
 */
void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw Text
	FramesPerSec();

	DrawText();
	
	glutSwapBuffers();
}

void Renderer::SetXRotation(float aXRotation)
{
	this->iXRotation = aXRotation;
}

void Renderer::SetYRotation(float aYRotation)
{
	this->iYRotation = aYRotation;
}

float Renderer::GetXRotation()
{
	return this->iXRotation;
}

float Renderer::GetYRotation()
{
	return this->iYRotation;
}

void Renderer::SetOldXRotation(float aXRotation)
{
	this->iOldXRotation = aXRotation;
}

void Renderer::SetOldYRotation(float aYRotation)
{
	this->iOldYRotation = aYRotation;
}

float Renderer::GetOldXRotation()
{
	return this->iOldXRotation;
}

float Renderer::GetOldYRotation()
{
	return this->iOldYRotation;
}

void Renderer::SetXPosition(float aXPosition)
{
	this->iXPosition = aXPosition;
}

void Renderer::SetYPosition(float aYPosition)
{
	this->iYPosition = aYPosition;
}

float Renderer::GetXPosition()
{
	return this->iXPosition;
}

float Renderer::GetYPosition()
{
	return this->iYPosition;
}

void Renderer::SetZoom( float aZoom )
{
	if ( (aZoom > -33.0f ) && ( aZoom < -15.0f ) )
	{
		this->iZoom = aZoom;
	}
	
}
float Renderer::GetZoom()
{
	return this->iZoom;
}

float Renderer::GetScreenHeightInPixels()
{
	return this->iScreenHeight;
}

//-----------------------------
//resize the window
//-----------------------------
void Renderer::ResizeScene(const int aWidth, const int aHeight)
{
	glViewport( 0, 0, aWidth, aHeight );

	// Projection transformation
	glMatrixMode(GL_PROJECTION); // Specifies which matrix stack is the target for matrix operations
	glLoadIdentity(); // We initialize the projection matrix as identity
	gluPerspective(60.0, (GLfloat)aWidth/(GLfloat)aHeight, 1.5, 500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	this->iScreenHeight = aHeight;
	// This command redraw the scene (it calls the same routine of glutDisplayFunc)
	//glutPostRedisplay ();
}

//EXTERNAL FUNCTIONS TO USE GLUT CALLBACKS
//-----------------------------------------------------
void RenderSceneWithRenderer()
{
	if (Renderer::iCurrentRenderer != NULL)
		Renderer::iCurrentRenderer->RenderScene();
}

void ResizeSceneWithRenderer( int aWidth, int aHeight )
{
	if (Renderer::iCurrentRenderer != NULL)
		Renderer::iCurrentRenderer->ResizeScene(aWidth, aHeight);
}

void RenderSceneWithRendererTimerFunc(int timeValue)
{
	glutPostRedisplay();
	glutTimerFunc(timeValue, RenderSceneWithRendererTimerFunc, timeValue);
}




