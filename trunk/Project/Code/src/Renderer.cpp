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
#include "Porcelain.h"
#include "Diffuse.h"
#include "Mesh.h"
#include "OBJMeshLoader.h"
#include "SShapedCurveToneMapper.h"
#include "PhotographicToneMapper.h"
#include "OpenGLUtility.h"
#include "BloomEffect.h"
#include "UIHandler.h"
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
, iMaterialIndex( 0 )
, iObjFileNameIndex( 0 )
, iLightEnvironmentIndex( 0 )
{
	InitMain();
	Renderer::iCurrentRenderer = this;
	iCubeMap = new CubeMap( 4 );
	iDiffuseConvCubeMap = new CubeMap( 4 );
	GetTrackball().SetResolution( 1.0 );
	iMenu = new GLMenu(KTextureHeight, KTextureWidth, (int)GLUT_BITMAP_HELVETICA_12);
	
	iMenu->addMenuLine("(e/E) Next/Previous Light Environment");
	iMenu->addMenuLine("(m/M) Next/Previous Material");
	iMenu->addMenuLine("(f/F) Increase/Decrease Fresnel's Eta");
	iMenu->addMenuLine("(b/B) Blooming");
	iMenu->addMenuLine("(o/O) Next/Previous Model");
	iMenu->addMenuLine("(q) Quit");
	iMenu->addMenuLine("(h) Toggle Help Menu");
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
	delete iMenu;

	glDeleteLists(iObject, 1);

	delete[] iMaterials;
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

void Renderer::LoadLightEnvironmentFileNames( const string& aEnvDirectoryFileName,
											  const string& aLightEnvFileName,
											  std::vector< std::string >& aReflectFileNames, 
											  std::vector< std::string >& aDiffuseConvFileNames, 
											  const std::string aDirectoryPrefix )
{
	ifstream DirectoryFile;
	DirectoryFile.open(aEnvDirectoryFileName.c_str(), ifstream::in);

	// if the file containing the directory names has been opened
	if (DirectoryFile.is_open())
	{
		// while input is good
		while (DirectoryFile.good())
		{
			// get a complete line
			std::string strDirectory;

			std::getline( DirectoryFile, strDirectory );

			strDirectory = aDirectoryPrefix + strDirectory + "/";

			// see if the lighting environment file containing the actual names can be opened
			ifstream lightEnv;
			lightEnv.open( (strDirectory + aLightEnvFileName).c_str(), ifstream::in);

			// if the lightEnv.txt exists
			if ( lightEnv.is_open() )
			{
				std::string strReflectMapName;
				std::string strDiffuseConvMapName;

				// while input is good
				if (lightEnv.good())
				{
					// get a complete line
					std::getline( lightEnv, strReflectMapName );
				}

				if ( lightEnv.good() )
				{
					// get a complete line
					std::getline( lightEnv, strDiffuseConvMapName );
				}

				// close it and store check if the cubemaps exist
				lightEnv.close();


				if (CheckExistanceCubeMap( strDirectory + strReflectMapName, strDirectory + strDiffuseConvMapName  ))
				{
					aReflectFileNames.push_back( strDirectory + strReflectMapName );
					aDiffuseConvFileNames.push_back( strDirectory + strDiffuseConvMapName );
				}
			}
		}

		// only need to close the file if it was opened correctly
		DirectoryFile.close();
	}
}

bool Renderer::CheckExistanceCubeMap(const string& aStrReflectMapName,
									 const string& aStrDiffuseConvMapName)
{
	// now test to see if the reflectance cubemap and diffuse convolution cubemap can be found
	bool LightEnvOK = true;

	for (int i = 0; i < 6; i++)
	{
		ifstream lightEnvFile;
		lightEnvFile.open( (aStrReflectMapName + suffixes[i]).c_str(), ifstream::in);
		// if the directory file name is open
		if (lightEnvFile.is_open())
		{
			lightEnvFile.close();
		}
		else
		{
			LightEnvOK = false;
		}

		ifstream lightDiffuseConvEnvFile;
		lightDiffuseConvEnvFile.open( (aStrDiffuseConvMapName + suffixes[i]).c_str(), ifstream::in);
		// if the directory file name is open
		if (lightDiffuseConvEnvFile.is_open())
		{
			lightDiffuseConvEnvFile.close();
		}
		else
		{
			LightEnvOK = false;
		}

	}

	return LightEnvOK;
}

/** \brief Method that creates a mesh
 *
 *   Creates an instance of a desired mesh type
 */
void Renderer::CreateScene()
{
	LoadLightEnvironmentFileNames("./textures/lightEnvironments.txt",
								  "lightEnv.txt",
								  iEnvironmentFileNames,
								  iDiffuseConvFileNames,
								  "./textures/");
	if (iEnvironmentFileNames.empty() || iDiffuseConvFileNames.empty())
	{
		cout << "Could not find any lighting environment." << endl;
		exit( -1 );
	}

	iCubeMap->setupCompressedCubeMap( iEnvironmentFileNames[iLightEnvironmentIndex] );
	//iCubeMap->setupCompressedCubeMap("./textures/stpeters/stpeters512", minExponent, maxExponent);
	//iCubeMap->setupCompressedCubeMap("./textures/tappanYard/tappanYard", minExponent, maxExponent);
	//iCubeMap->setupCompressedCubeMap("./textures/pisa/pisa", minExponent, maxExponent);

	iDiffuseConvCubeMap->setupCompressedCubeMap( iDiffuseConvFileNames[iLightEnvironmentIndex] );


	iBloomEffect = new  BloomEffect();

	iUIHandler->AddKeyListener( iBloomEffect );

	/************************************************************************/
	/* XXX:                                                                 */
	/************************************************************************/
	this->iLuminanceConverter = new LuminanceConverter(KTextureWidth, KTextureHeight, -1, -1);

	// Setup Scene Texture
	this->iScreenCapture = new ScreenCapture(KTextureHeight, KTextureWidth);

	this->iToneMapper = new PhotographicToneMapper(0, 0, KTextureWidth, KTextureHeight);
	this->iSCurveToneMapper = new SShapedCurveToneMapper(0, 0, KTextureWidth, KTextureHeight);

	this->iMaterials[0] = new IBLPerfectReflection( "./shader/iblreflection", iCubeMap->getCubeMapId() );
	this->iMaterials[1] = new IBLRefraction(1.5f, 1.0f, "./shader/iblrefraction", iCubeMap->getCubeMapId());
	this->iMaterials[2] = new Diffuse( "./shader/diffuse", iDiffuseConvCubeMap->getCubeMapId() );
	this->iMaterials[3] = new Porcelain(1.333f, 1.0f, "./shader/porcelain", iCubeMap->getCubeMapId(), iDiffuseConvCubeMap->getCubeMapId() );

	iUIHandler->AddKeyListener( static_cast<IBLRefraction*>(this->iMaterials[1]) );

	// load object file names from file
	OBJLoader::loadModelFileNames("./3ds/models.txt", iObjFileNames, "./3ds/");
	// if the list is empty, then create a simple sphere to represent the object
	if ( iObjFileNames.empty() )
	{
		iObject = CreateDisplayListSphere();
	}
	else
	{
		iObject = CreateDisplayList( iObjFileNames[0] );
	}

}

GLuint Renderer::CreateDisplayListSphere()
{
	GLuint displayListId = 0;

	GLUquadricObj* quadricObj = gluNewQuadric();
	gluQuadricDrawStyle(quadricObj, GLU_FILL);
	gluQuadricNormals(quadricObj, GLU_SMOOTH);
	gluQuadricTexture(quadricObj, GL_TRUE);

	// Create the id for the list
	displayListId = glGenLists(1);

	// start list
	glNewList( displayListId, GL_COMPILE );

	// call the function that contains the rendering commands
	gluSphere(quadricObj, 0.5, 128, 128);

	// endList
	glEndList();

	gluDeleteQuadric(quadricObj);

	return displayListId;
}

GLuint Renderer::CreateDisplayList( const string& aObjFilename )
{
	GLuint displayListId = 0;

	std::cout << "Loading model: \"" << aObjFilename << "\". Please wait... " << std::endl;
	Mesh* mesh = OBJLoader::loadModel( aObjFilename );
	std::cout << "Done!" << std::endl;

	if ( mesh != NULL )
	{
		// Create the id for the list
		displayListId = glGenLists(1);

		// start list
		glNewList( displayListId, GL_COMPILE );

		// call the function that contains the rendering commands
		mesh->draw();

		// endList
		glEndList();
	}

	return displayListId;
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
	//iScreenCapture->startCapture();
	iBloomEffect->Begin();

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

		iMaterials[iMaterialIndex]->start();
		{
			glCallList( iObject );
		}
		iMaterials[iMaterialIndex]->stop();

	}
	glPopMatrix();

	// unbind and render
	//GLuint capturedSceneTexId = iScreenCapture->stopCapture();
	GLuint capturedSceneTexId = iBloomEffect->End();

	GLuint finalScene = iLuminanceConverter->processData(capturedSceneTexId);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iToneMapper->toneMap(capturedSceneTexId, finalScene);
	//iSCurveToneMapper->toneMap(capturedSceneTexId, finalScene);
	
	//RenderSceneOnQuad(capturedSceneTexId, GL_TEXTURE_2D, 512, 512);

	//iScreenCapture->renderToScreen();

	// Draw Text
	FramesPerSec();

	//DrawText();

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

void Renderer::setUIHandler(UIHandler* aUIHandler)
{
	iUIHandler = aUIHandler;
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

	unsigned int numberOfEnvironments = 0;
	unsigned int numberOfObjects = 0;

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
	case 'e':
		ToggleEnvironment( false );
		break;
	case 'E':
		ToggleEnvironment( true );
		break;
	case 'm':
		iMaterialIndex = (iMaterialIndex + 1) % KNumberOfMaterials;

		iMaterials[iMaterialIndex]->setEnvironmentCubeMap( iCubeMap->getCubeMapId() );
		iMaterials[iMaterialIndex]->setDiffuseConvCubeMap( iDiffuseConvCubeMap->getCubeMapId() );
		break;
	case 'M':
		iMaterialIndex = (iMaterialIndex + (KNumberOfMaterials - 1) ) % KNumberOfMaterials;

		iMaterials[iMaterialIndex]->setEnvironmentCubeMap( iCubeMap->getCubeMapId() );
		iMaterials[iMaterialIndex]->setDiffuseConvCubeMap( iDiffuseConvCubeMap->getCubeMapId() );
		break;
	case 'o': 
		numberOfObjects = iObjFileNames.size();
		// if there is more than one object, allow user to change object
		if ( iObjFileNames.size() > 1 )
		{
			iObjFileNameIndex = (iObjFileNameIndex + 1) % iObjFileNames.size();

			// delete current display list
			glDeleteLists( iObject, 1 );

			// and create new display list
			iObject = CreateDisplayList( iObjFileNames[iObjFileNameIndex] );
		}
		break;
	case 'O':
		// if there is more than one object, allow user to change object
		numberOfObjects = iObjFileNames.size();

		if ( numberOfObjects > 1 )
		{
			iObjFileNameIndex = (iObjFileNameIndex + (numberOfObjects - 1)) % numberOfObjects;

			// delete current display list
			glDeleteLists( iObject, 1 );

			// and create new display list
			iObject = CreateDisplayList( iObjFileNames[iObjFileNameIndex] );
		}
		break;
	case 'q':
		exit( 0 );
	    break;
	case 'h':
		iShowMenu = ! iShowMenu;
		break;
	}
}

/**
 * @param next
 */
void Renderer::ToggleEnvironment(bool next)
{
	unsigned int numberOfEnvironments = 0;
	unsigned int numberOfObjects = 0;

	numberOfEnvironments = iEnvironmentFileNames.size();

	if (numberOfEnvironments > 1 )
	{
		if ( next )
		{
			iLightEnvironmentIndex = (iLightEnvironmentIndex + 1) % numberOfEnvironments;
		}
		else
		{
			iLightEnvironmentIndex = (iLightEnvironmentIndex + (numberOfEnvironments - 1)) % numberOfEnvironments;
		}

		delete iCubeMap;
		delete iDiffuseConvCubeMap;

		iCubeMap = new CubeMap( 4 );
		iDiffuseConvCubeMap = new CubeMap( 4 );

		iCubeMap->setupCompressedCubeMap( iEnvironmentFileNames[iLightEnvironmentIndex] );
		iDiffuseConvCubeMap->setupCompressedCubeMap( iDiffuseConvFileNames[iLightEnvironmentIndex] );

		iMaterials[iMaterialIndex]->setEnvironmentCubeMap( iCubeMap->getCubeMapId() );
		iMaterials[iMaterialIndex]->setDiffuseConvCubeMap( iDiffuseConvCubeMap->getCubeMapId() );
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




