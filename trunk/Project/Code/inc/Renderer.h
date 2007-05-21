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
#include "IMouseListener.h"
#include "IKeyListener.h"
#include "GLMenu.h"


class VirtualTrackball;
class CubeMap;
class FrameBufferObject;
class ScreenCapture;
class LuminanceConverter;
class PhotographicToneMapper;
class SShapedCurveToneMapper;

class Material;
class IBLPerfectReflection;
class IBLRefraction;
class Porcelain;
class Diffuse;

class Mesh;
class BloomEffect;
class UIHandler;

//CONSTANTS:
const int KNumberOfColorMaps = 3;
const int KNumberOfShaderPrograms = 3;
const int KNumberOfMaterials = 4;

//CLASS DECLARATION

/** \brief Rendering class.
*
* This class contains all the functions necessary for rendering.
*/
class Renderer: public IMouseListener, public IKeyListener
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

	void SetZoom(float aZoom);
	float GetZoom();

	float GetScreenHeightInPixels();

	// Mouse handling functions
	virtual void ProcessMouseMotionEvent( int x, int y );
	virtual void ProcessMouseEvent( int button, int state, int x, int y );

	// Key handling events
	virtual void ProcessNormalKeys( unsigned char key, int x, int y );
	virtual void ProcessCursorKeys( int key, int x, int y );


	//GETTERS
	//---------------------------------------
	VirtualTrackball& GetTrackball();

	//SETTERS
	//---------------------------------------
	void setUIHandler(UIHandler* aUIHandler);

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

	GLuint CreateDisplayListSphere();

	GLuint CreateDisplayList( const string& aObjFilename );


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
	float iZoom;
	float iScreenHeight;
	int iMouseButtonDown;
	int iMouseX;
	int iMouseY;
	int iZoomMouseY;

	UIHandler* iUIHandler;
	// Menu
	GLMenu* iMenu;
	bool iShowMenu;

	CubeMap* iCubeMap;
	CubeMap* iDiffuseConvCubeMap;

	BloomEffect* iBloomEffect;
	ScreenCapture* iScreenCapture;
	LuminanceConverter* iLuminanceConverter;
	PhotographicToneMapper* iToneMapper;
	SShapedCurveToneMapper* iSCurveToneMapper;

	Material* iMaterials[KNumberOfMaterials];
	int iMaterialIndex;
	VirtualTrackball trackball;

	vector<string> iObjFileNames;
	int iObjFileNameIndex;
	GLuint iObject;
};


//EXTERNAL FUNCTIONS
/// Reference to rendering function. Used to pass the render object's rendering function to OpenGL.
extern void RenderSceneWithRenderer();
///Reference to resize function. Used to pass the render object's resize function to OpenGL.
extern void ResizeSceneWithRenderer( int aWidth, int aHeight );
/// Reference to timer function.
extern void RenderSceneWithRendererTimerFunc(int timeValue);

#endif
