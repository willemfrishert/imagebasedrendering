//INCLUDES
#include "Renderer.h"
#include "Vertex.h"
#include "tga.h"
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

// GL ERROR CHECK
int CheckGLError(char *aFile, int aLine)
{
	int result = 0;
	GLenum glError = glGetError();

	while (glError != GL_NO_ERROR)
	{
		cout << "Error in file " << aFile << " at line: " << aLine << endl;
		cout << "GL Error #" << glError << ": " << gluErrorString(glError) << endl << endl;
		result = 1;
		glError = glGetError();
	}

	return result;
}

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
	delete iShaderProgram;
//	delete iVertexShader;
	delete iFragmentShader;
}

void Renderer::InitShaders()
{
	//iVertexShader = new ShaderObject(GL_VERTEX_SHADER, "shader/retinex.vert");
	iFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "shader/retinex.frag");

	iShaderProgram = new ShaderProgram();

	//iShaderProgram->attachShader( *iVertexShader );
	iShaderProgram->attachShader( *iFragmentShader );
	iShaderProgram->buildProgram();
	iShaderProgram->useProgram();
}

void Renderer::LoadTextures()
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//iTextureId[0] = loadTGATexture("textures/circle.tga");
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	////iHDRImageDecoded = new HDRLoaderResult;
	////HDRLoader::load("./textures/stpeters_cross.hdr", *iHDRImageDecoded);

	iHDRImageEncoded = new HDRLoaderResultEncoded;
	HDRLoader::load("./textures/stpeters_negz.hdr", *iHDRImageEncoded);

	// generate texture
	glGenTextures (1, &iTextureId[0]);
	glBindTexture (GL_TEXTURE_2D, iTextureId[0]);

	// setup some parameters for texture filters and mipmapping
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16,
		iHDRImageEncoded->width, iHDRImageEncoded->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, iHDRImageEncoded->cols);
	
	////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iHDRImageDecoded->width, iHDRImageDecoded->height, 0,
	////			 GL_RGB, GL_FLOAT, iHDRImageDecoded->cols);
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
	LoadTextures();

	InitLights();


	iBloomEffect = new BloomEffect();
//	InitShaders();
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
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


// ********** GL DRAWING METHODs *******


// *************	RENDERING METHODS *********** /


void Renderer::InitLights()
{
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	//CREATE THE LIGHT
	GLfloat light0_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light0_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light0_specular[] ={ 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0_position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

//	glLightfv (GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
//	glLightfv (GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
}

/** 
 *\brief Method that specifies how the screen is rendered
 */
void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CheckGLError( __FILE__, __LINE__ );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	iBloomEffect->Begin();

	RenderSceneOnQuad(iTextureId[0]);

	GLuint texture = iBloomEffect->End();

	//RenderSceneOnQuad(texture);

	CheckGLError( __FILE__, __LINE__ );

	// Draw Text
	FramesPerSec();

	DrawText();
	
	glutSwapBuffers();
}

void Renderer::RenderSceneOnQuad( GLuint & aTexture )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( -1, 1, -1, 1 );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, aTexture );

	//RENDER MULTITEXTURE ON QUAD
	glBegin(GL_QUADS);
	glTexCoord2i( 0, 0 );
	glVertex2f( -1.0f, -1.0f );

	glTexCoord2i( 1, 0 );
	glVertex2f( 1.0f, -1.0f );

	glTexCoord2i( 1, 1 );
	glVertex2f( 1.0f, 1.0f );

	glTexCoord2i( 0, 1 );
	glVertex2f( -1.0f, 1.0f );
	glEnd();

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
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




