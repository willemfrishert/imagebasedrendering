
//INCLUDE FILES
#include "CubeMap.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "VirtualTrackball.h"
#include "Renderer.h"
#include "IMouseListener.h"
#include "IKeyListener.h"
#include "UIHandler.h"

//CONSTANTS
#ifdef USE_RADIANS
const float KAngleChangeAmount = 0.001f;
#elif defined USE_DEGREES
const float KAngleChangeAmount = 0.05f;
#endif

//INIT STATIC DATA
UIHandler* UIHandler::iCurrentUi = 0;

//Constructor, saves a pointer to the renderer
UIHandler::UIHandler( Renderer* aRenderer )
{
	iRenderer = aRenderer;
	UIHandler::iCurrentUi = this;
	/*GetTrackball().SetResolution( 1.0 );*/
}

//Destructor
UIHandler::~UIHandler()
{
}

//Used to handle the keyboard input (ASCII Characters)
void UIHandler::ProcessNormalKeys(unsigned char key, int x, int y)
{
	// Call all the listeners
	list<IKeyListener*>::iterator it = iKeyListeners.begin();
	list<IKeyListener*>::iterator end = iKeyListeners.end();

	for (; it != end; it++)
	{
		(*it)->ProcessNormalKeys(key, x, y);
	}	
}

// Used to handle the keyboard input (not ASCII Characters)
void UIHandler::ProcessCursorKeys(int key, int x, int y)
{
	//switch (key)
	//{
	//	//MODIFY X
	//case GLUT_KEY_UP:
	//	break;
	//case GLUT_KEY_DOWN:
	//	break;
	//	//MODIFY Y
	//case GLUT_KEY_LEFT:
	//	break;
	//case GLUT_KEY_RIGHT:
	//	break;
	//	//MODIFY Z
	//case GLUT_KEY_PAGE_UP:
	//	break;
	//case GLUT_KEY_PAGE_DOWN:
	//	break;
	//	//RESET Base angle as well
	//case GLUT_KEY_HOME:
	//	break;
	//	//FREEZE
	//default:
	//	break;
	//}
	// Call all the listeners
	list<IKeyListener*>::iterator it = iKeyListeners.begin();
	list<IKeyListener*>::iterator end = iKeyListeners.end();

	for (; it != end; it++)
	{
		(*it)->ProcessCursorKeys(key, x, y);
	}
}

void UIHandler::ProcessMouseEvent( int button, int state, int x, int y )
{
	// Call all the listeners
	list<IMouseListener*>::iterator it = iMouseListeners.begin();
	list<IMouseListener*>::iterator end = iMouseListeners.end();

	for (; it != end; it++)
	{
		(*it)->ProcessMouseEvent(button, state, x, y);
	}

}

void UIHandler::ProcessMouseMotionEvent( int x, int y )
{
	// Call all the listeners
	list<IMouseListener*>::iterator it = iMouseListeners.begin();
	list<IMouseListener*>::iterator end = iMouseListeners.end();

	for (; it != end; it++)
	{
		(*it)->ProcessMouseMotionEvent(x, y);
	}
	
}

/**
 * @param listener
 */
void UIHandler::AddMouseListener(IMouseListener* listener)
{
	list<IMouseListener*>::iterator begin = iMouseListeners.begin();
	list<IMouseListener*>::iterator end = iMouseListeners.end();

	// if not found
	if( find(begin, end, listener) == end)
	{
		iMouseListeners.push_back( listener );
	}
}

/**
 * @param listener
 */
void UIHandler::RemoveMouseListener(IMouseListener* listener)
{
	iMouseListeners.remove( listener );
}

/**
* @param listener
*/
void UIHandler::AddKeyListener(IKeyListener* listener)
{
	list<IKeyListener*>::iterator begin = iKeyListeners.begin();
	list<IKeyListener*>::iterator end = iKeyListeners.end();

	// if not found
	if( find(begin, end, listener) == end)
	{
		iKeyListeners.push_back( listener );
	}
}

/**
* @param listener
*/
void UIHandler::RemoveKeyListener(IKeyListener* listener)
{
	iKeyListeners.remove( listener );
}


//EXTERNAL FUNCTIONS TO USE GLUT CALLBACKS
void ProcessNormalKeysWithUi( unsigned char key, int x, int y )
{
	if (UIHandler::iCurrentUi != 0 )
		UIHandler::iCurrentUi->ProcessNormalKeys( key, x, y  );
}

void ProcessCursorKeysWithUi( int key, int x, int y )
{
	if (UIHandler::iCurrentUi != 0 )
		UIHandler::iCurrentUi->ProcessCursorKeys( key, x, y );
}
void ProcessMouseEventWithUi( int button, int state, int x, int y)
{
	if (UIHandler::iCurrentUi != 0 )
		UIHandler::iCurrentUi->ProcessMouseEvent( button, state, x, y );
}

void ProcessMouseMotionEventWithUi( int x, int y )
{
	if (UIHandler::iCurrentUi != 0 )
		UIHandler::iCurrentUi->ProcessMouseMotionEvent( x, y );
}

