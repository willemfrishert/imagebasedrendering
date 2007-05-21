#include "CubeMap.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "VirtualTrackball.h"
#include "Renderer.h"
#include "UIHandler.h"

#include "GPUParallelReductor.h"

#include <math.h>

//CONSTANTS
const int KWindowPositionX = 50;
const int KWindowPositionY = 50;
const int KWindowWidth = 512;
const int KWindowHeight = 512;


void initContext(int argc, char **argv)
{
	glutInit( &argc, argv );
	
	//Init Window
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition( KWindowPositionX, KWindowPositionY );
	glutInitWindowSize( KWindowWidth, KWindowHeight ); //800,600);
	glutCreateWindow("Real-Time HDR Image Based Lighting");

	//Set some Glut functions
	glutKeyboardFunc( ProcessNormalKeysWithUi );
	glutSpecialFunc(  ProcessCursorKeysWithUi );
	glutMouseFunc(    ProcessMouseEventWithUi );
	glutMotionFunc(   ProcessMouseMotionEventWithUi );

	glutDisplayFunc(  RenderSceneWithRenderer );
	//glutIdleFunc(     RenderSceneWithRenderer );
	glutReshapeFunc(  ResizeSceneWithRenderer );
	glutTimerFunc(30, RenderSceneWithRendererTimerFunc, 30);

#ifdef _WIN32
	GLenum err = glewInit();
#endif	
}

void testGPU()
{
	const int side = 512;
	//float f[side*side];
	float* f = new float[side*side];
	for (int i = 0; i < side*side; i++)
	{
		f[i] = 1;
		//f[i] = (i % 2) + 1;
		//f[i] = i % 2 ? 0.25f : 0.5f;
	}

	//f[side] = 1056;
	//f[side] = 0.75f;
	//f[side*2] = 0.6f;
	//f[side*2+side/2] = 0.3333f;
	//f[side*side - side/2] = 0.8f;

	GPUParallelReductor proc(f, side, side, "./shader/logSumFP.frag");
	GLfloat result[4];
	proc.processData(result);
	delete f;
}

/**
* Main method.
* This method is called first an it will do the following:
*  -# Creates the renderer and ui objects
*  -# Initializes Glut
*  -# Calls renderer->CreateMesh to create a CMesh
*  -# Starts the glutMainLoop()
* @param argc amount of arguments for glutInit().
* @param argv arguments for glutInit().
* @return 0
*/
int main(int argc, char **argv)
{	
	//Initialize GL using GLUT functions
	initContext(argc, argv);

	//testGPU();


	//Init renderer
	Renderer* renderer = new Renderer();
	UIHandler* ui = new UIHandler( NULL );
	ui->AddMouseListener( renderer );
	ui->AddKeyListener( renderer );

	renderer->setUIHandler( ui );
	renderer->CreateScene();

	//glutFullScreen();

	//START GL loop
	glutMainLoop();

	delete ui;
	delete renderer;
	return 0;
}
