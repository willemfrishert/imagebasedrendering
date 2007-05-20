//INCLUDES
#include "CubeMap.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "VirtualTrackball.h"
#include "Renderer.h"
#include "Vertex.h"
#include "FrameBufferObject.h"
#include "ScreenCapture.h"
#include "LuminanceConverter.h"
#include "GPUParallelReductor.h"
#include "IBLPerfectReflection.h"
#include "IBLRefraction.h"
#include "Mesh.h"
#include "OBJMeshLoader.h"
#include "SShapedCurveToneMapper.h"
#include "PhotographicToneMapper.h"
#include "GLMenu.h"


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
, iMouseX( 0 )
, iMouseY( 0 )
, iZoom( 0 )
, iMouseButtonDown( GLUT_UP )
, iShowMenu( false )
{
	InitMain();
	Renderer::iCurrentRenderer = this;
	iCubeMap = new CubeMap( 4 );
	GetTrackball().SetResolution( 1.0 );
	iMenu = new GLMenu(KTextureHeight, KTextureWidth, (int)GLUT_BITMAP_HELVETICA_18);
	
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
	iMenu->addMenuLine("(m/M) Loop Materials");
}


//
//DESTRUCTOR releases the used heap
//
Renderer::~Renderer()
{
	delete iScreenCapture;
	delete iLuminanceConverter;
	delete iCubeMap;
	delete iToneMapper;
	delete iIBLReflection;
	delete iIBLRefraction;
	delete iMenu;

	if (iDragon)
	{
		delete iDragon;
	}

	if (pObj)
	{
		gluDeleteQuadric(pObj);
	}
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
	unsigned char minExponent;
	unsigned char maxExponent;
	iCubeMap->setupCompressedCubeMap("./textures/stpeters/stpeters512", minExponent, maxExponent);
	/*iCubeMap->setupCompressedCubeMap("./textures/tappanYard/tappanYard", minExponent, maxExponent);*/
	//iCubeMap->setupCompressedCubeMap("./textures/pisa/pisa", minExponent, maxExponent);

	//iCubeMap->setupCubeMap("./textures/rnl");
	//iCubeMap->setupCubeMap("./textures/stpeters");

	/************************************************************************/
	/* XXX:                                                                 */
	/************************************************************************/
	this->iLuminanceConverter = new LuminanceConverter(KTextureWidth, KTextureHeight, -1, -1);
	
	// Setup Scene Texture
	this->iScreenCapture = new ScreenCapture(KTextureHeight, KTextureWidth);

	this->iToneMapper = new PhotographicToneMapper(0, 0, KTextureWidth, KTextureHeight);
	this->iSCurveToneMapper = new SShapedCurveToneMapper(0, 0, KTextureWidth, KTextureHeight);

	this->iIBLReflection = new IBLPerfectReflection("./shader/iblreflection");
	this->iIBLRefraction = new IBLRefraction(1.5f, 1.0f, "./shader/iblrefraction");

	pObj = gluNewQuadric();

	gluQuadricDrawStyle(pObj, GLU_FILL);
	gluQuadricNormals(pObj, GLU_SMOOTH);
	gluQuadricTexture(pObj, GL_TRUE);

	createDragonDL();
}

void Renderer::createDragonDL() 
{
	//iDragon = OBJLoader::loadModel("./3ds/2HeadedDragon50KSmooth.obj");
	iDragon = OBJLoader::loadModel("./3ds/teapot.obj");

	// Create the id for the list
	iDragonDL = glGenLists(1);

	// start list
	glNewList(iDragonDL,GL_COMPILE);

		// call the function that contains the rendering commands
		iDragon->draw();

	// endList
	glEndList();
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
	float x( -0.9f ), y( -0.9f );
	
	//configure the transforms for 2D
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	{
		glLoadIdentity();

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		{
				glLoadIdentity();
				gluOrtho2D( -1.0f, 1.0f, -1.0f, 1.0f);
			
				glDisable(GL_LIGHTING);
				glDisable( GL_TEXTURE_2D );
				glDisable(GL_DEPTH_TEST);
			
				glColor3f(1.0, 0.0, 0.0);
				glRasterPos2f(x, y);
			
				for (int i = 0, len = static_cast<int>( strlen(iFpsCountString) ); i < len; i++)
				{
					glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, iFpsCountString[i] );
				}
				//glFlush();
				glEnable(GL_LIGHTING);
				glEnable(GL_DEPTH_TEST);
		}
		glPopMatrix(); // PROJECTION
	}
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix(); // MODELVIEW
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
	static Matrix4 trackballMatrix;
	static float trackballMatrixGL[16];

	// Setup FBO to render the scene to a texture
	iScreenCapture->startCapture();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	{
		/************************************************************************/
		/* OPTMIZE PLEASE!!!                                                    */
		/************************************************************************/
		glPushMatrix();
		{
			trackballMatrix = trackball.getRotationMatrix( );
			trackballMatrix.getGLMatrix( trackballMatrixGL );
			
			float inverseGL[16];
			Matrix4 inverse = Matrix4::inverse( trackballMatrix );
			inverse.getGLMatrix( inverseGL );

			//glMultMatrixf( inverseGL );
			//iCubeMap->render( trackballMatrixGL );

			glMultMatrixf( trackballMatrixGL );
			iCubeMap->render( inverseGL );
		}
		glPopMatrix();

		glTranslatef(0, 0, iZoom);
		glMultMatrixf( trackballMatrixGL );

		//iIBLReflection->start( iCubeMap->getCubeMapId() );
		iIBLRefraction->start( iCubeMap->getCubeMapId() );
		{
			//gluSphere(pObj, 0.5, 128, 128);
			glCallList(iDragonDL);
		}
		//iIBLReflection->stop();
		iIBLRefraction->stop();
		
	}
	glPopMatrix();

	// unbind and render
	GLuint capturedSceneTexId = iScreenCapture->stopCapture();

	GLuint finalScene = iLuminanceConverter->processData(capturedSceneTexId);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iToneMapper->toneMap(capturedSceneTexId, finalScene);
	//iSCurveToneMapper->toneMap(capturedSceneTexId, finalScene);

	//RenderSceneOnQuad(capturedSceneTexId, GL_TEXTURE_2D, 512, 512);

	//iScreenCapture->renderToScreen();

	// Draw Text
	FramesPerSec();

	DrawText();

	// Draw Menu
	if (iShowMenu)
	{
		iMenu->draw();
	}
	
	
	glutSwapBuffers();
}

void Renderer::RenderSceneOnQuad(GLuint textureId, GLenum target, 
								 int width, int height)
{
	glEnable( target );
	
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	{
		glLoadIdentity();
		gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f);

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		{
			glLoadIdentity();

			glBindTexture(target, textureId);
			glBegin(GL_QUADS);
			{
				glTexCoord2i(0, 0);
				glVertex2f(0.0f, 0.0f);
				
				glTexCoord2i(1, 0);
				glVertex2f(1.0f, 0.0f);
				
				glTexCoord2i(1, 1);
				glVertex2f(1.0f, 1.0f);
				
				glTexCoord2i(0, 1);
				glVertex2f(0.0f, 1.0f);

			}
			glEnd();
		}
		glPopMatrix();

		glMatrixMode( GL_PROJECTION );
	}
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	
	glDisable( target );
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

VirtualTrackball& Renderer::GetTrackball()
{
	return this->trackball;
}

/**
 * @param button
 * @param state
 * @param x
 * @param y
 */
void Renderer::ProcessMouseEvent( int button, int state, int x, int y )
{
	iMouseY		= y;
	iMouseX		= x;
	
	// needs to be stored also on the mouse motion
	iZoomMouseY = y;

	iMouseButtonDown = button;

	//Set point of rotation
	if (GLUT_LEFT_BUTTON == button)
	{
		if(GLUT_DOWN == state)
		{
			//iMouseButtonDown = true;
			SetOldXRotation( GetXRotation() );
			SetOldYRotation( GetYRotation() );
			GetTrackball().MouseDown(Vector3<float>(x, y, 0));
		}
		else
		{
			GetTrackball().MouseUp(Vector3<float>(x, y, 0));
		}
	}
}

void Renderer::ProcessMouseMotionEvent( int x, int y )
{
	if( iMouseButtonDown == GLUT_LEFT_BUTTON )
	{
		SetXRotation( GetOldXRotation() 
			+ (float)(y - iMouseY) / 4.0);
		SetYRotation( GetOldYRotation() 
			+ (float)(x - iMouseX) / 4.0);

		GetTrackball().MouseMove(Vector3<float>(x, y, 0));
	}
	else if ( iMouseButtonDown == GLUT_RIGHT_BUTTON  )
	{
		float factor = 0.07f;

		if ( y - iZoomMouseY < 0 ) { factor *= -1; }

		// increase always by the same amount
		iZoom += factor;

		iZoomMouseY = y;

		// clamp the values
		iZoom = iZoom > 1.0f ? 1.0f : iZoom;
		iZoom = iZoom < -5.0f ? -5.0f : iZoom;
	}
}

void Renderer::ProcessCursorKeys(int key, int x, int y )
{
}

void Renderer::ProcessNormalKeys(unsigned char key, int x, int y )
{
	static float exposureIncrement = 0.02f;

	switch( key )
	{
	case '+':
		iToneMapper->setExposure( iToneMapper->getExposure() + exposureIncrement);
		break;
	case '-':
		iToneMapper->setExposure( iToneMapper->getExposure() - exposureIncrement);
		break;
	case 'L':
		iToneMapper->setLogAverage( iToneMapper->getLogAverage() + exposureIncrement);
	    break;
	case 'l':
		iToneMapper->setLogAverage( iToneMapper->getLogAverage() - exposureIncrement);
	    break;
	case 'h':
		iShowMenu = ! iShowMenu;
		break;
	case 'q':
	default:
		exit( 0 );
	    break;
	}
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
	gluPerspective(60.0, (GLfloat)aWidth/(GLfloat)aHeight, 0.1, 500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	this->iScreenHeight = aHeight;
	// This command redraw the scene (it calls the same routine of glutDisplayFunc)
	//glutPostRedisplay ();

	this->trackball.SetWindowRect(aWidth, aHeight);
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




