#ifndef MYRENDERER_JOHANNES_H
#define MYRENDERER_JOHANNES_H

#include "Basic.h"
#include "Mesh.h"
#include "ShaderObject.h"
#include "ShaderProgram.h"
#include "ShaderAttributeObject.h"
#include "ShaderUniformObject.h"
#include "ShaderUniformValue.h"
#include "ShaderUniformVector.h"
#include "glm_avl.h"


class VirtualTrackball;
class CubeMap;
class FrameBufferObject;
class ScreenCapture;
class LuminanceConverter;
class PhotographicToneMapper;
class IBLPerfectReflection;
class Mesh;

//CONSTANTS:
const int KNumberOfColorMaps = 3;
const int KNumberOfShaderPrograms = 3;

//CLASS DECLARATION

/** \brief Rendering class.
*
* This class contains all the functions necessary for rendering.
*/
class Renderer
{
	//PUBLIC FUNCTIONS
	//------------------
public:

	//CONSTRUCTORS
	//------------------

	/**
	* Default constructor.
	* Initializes private data, and sets the window size to ( KScreenWidth, KScreenHeight ).
	*/
	Renderer();

	//DESTRUCTOR
	//------------------
	/**
	* Default constructor.
	* Frees the reserved memory (iMesh).
	*/
	~Renderer();


	//MEMBER FUNCTIONS
	///Renders a scene. This function is called from OpenGL, and renders a scene.
	void RenderScene();

	///Resize the rendering window
	void ResizeScene(const int aWidth, const int aHeight);

	///Creates a Scene
	void CreateScene();

	///Sets the Trackball's X rotation
	void SetXRotation(float aXRotation);

	///Sets the Trackball's Y rotation
	void SetYRotation(float aYRotation);

	///Gets the Trackball's X rotation
	float GetXRotation();

	///Gets the Trackball's Y rotation
	float GetYRotation();

	///Sets the Trackball's X rotation
	void SetOldXRotation(float aXRotation);

	///Sets the Trackball's Y rotation
	void SetOldYRotation(float aYRotation);

	///Gets the Trackball's X rotation
	float GetOldXRotation();

	///Gets the Trackball's Y rotation
	float GetOldYRotation();

	///Sets the Trackball's X position
	void SetXPosition(float aXPosition);

	///Sets the Trackball's Y position
	void SetYPosition(float aYPosition);

	///Gets the Trackball's X position
	float GetXPosition();

	///Gets the Trackball's Y position
	float GetYPosition();

	void SetZoom(float aZoom);
	float GetZoom();

	float GetScreenHeightInPixels();

	void MouseMoved();

	//GETTERS
	//---------------------------------------
	VirtualTrackball& GetTrackball();

	float GetExposure();

	//SETTERS
	//---------------------------------------
	void SetExposure(float aValue);

	//PRIVATE FUNCTIONS
	//------------------
private:
	//Constructors will call this one
	void InitMain();

	// Initializes the shaders
	void InitShaders();

	//Render current object
	void RenderObject( );

	// FPS calculation. Frames drawn in a second.
	void FramesPerSec();

	/// Initializie OpenGL Lights
	void InitLights();

	/// Text output. Prints text on the screen.
	void DrawText() const;

	// loads the textures
	void LoadTextures();

	void SetupTexture(GLuint textureId, const GLvoid* data);

	void RenderSceneOnQuad(GLuint textureId, GLenum target, 
		int width, int height);


	//DRAWING FUNCTIONS
	//------------------

	void drawRoom();

	//PUBLIC STATIC DATA
	//------------------
public:
	/// A static pointer to the current renderer object.
	/// This is used to be able to pass rendering method to OpenGL.
	static Renderer* iCurrentRenderer;


	//PRIVATE DATA
	//------------------
private:
	char  iFpsCountString[ 60 ];	//< String for FPS

	float iCurrentTime;		//< Current time
	float iPreviousFrame;	///< The previous second
	float iPreviousTime;	///< The previous second

	int iFrame;				//< Current frame number (used to approximate FPS)

	// Trackball attributes
	float iXRotation;
	float iYRotation;
	float iOldXRotation;
	float iOldYRotation;
	float iXPosition;
	float iYPosition;
	float iZoom;
	float iScreenHeight;

	CubeMap* iCubeMap;

	ScreenCapture* iScreenCapture;
	LuminanceConverter* iLuminanceConverter;
	PhotographicToneMapper* iToneMapper;
	IBLPerfectReflection* iIBLReflection;

	VirtualTrackball trackball;

	GLUquadricObj* pObj;
	Mesh* iDragon3DS;
	GLMmodel* iDragonOBJ;
};


//EXTERNAL FUNCTIONS
/// Reference to rendering function. Used to pass the render object's rendering function to OpenGL.
extern void RenderSceneWithRenderer();
///Reference to resize function. Used to pass the render object's resize function to OpenGL.
extern void ResizeSceneWithRenderer( int aWidth, int aHeight );
/// Reference to timer function.
extern void RenderSceneWithRendererTimerFunc(int timeValue);

#endif
