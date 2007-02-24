
//INCLUDE FILES
#include "CubeMap.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "VirtualTrackball.h"
#include "Renderer.h"
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
	this->iRenderer->GetTrackball().SetResolution( 1.0 );
}

//Destructor
UIHandler::~UIHandler()
{
}

//Used to handle the keyboard input (ASCII Characters)
void UIHandler::ProcessNormalKeys(unsigned char key, int x, int y)
{
	if (key == 't')
	{
	}

	if (key == 'l')
	{
	}

	if (key == 'q')
	{
		exit( 0 );
	}
}

// Used to handle the keyboard input (not ASCII Characters)
void UIHandler::ProcessCursorKeys(int key, int x, int y)
{
	switch (key)
	{
		//MODIFY X
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_DOWN:
		break;
		//MODIFY Y
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
		//MODIFY Z
	case GLUT_KEY_PAGE_UP:
		break;
	case GLUT_KEY_PAGE_DOWN:
		break;
		//RESET Base angle as well
	case GLUT_KEY_HOME:
		break;
		//FREEZE
	default:
		break;
	}
}

void UIHandler::ProcessMouseEvent( int button, int state, int x, int y )
{
	iMouseY = y;
	iMouseX = x;

	switch (button)
	{
		//Set point of rotation
	case GLUT_LEFT_BUTTON:
		if(GLUT_DOWN == state)
		{
			iMouseButtonDown = EMouseDownLeft;
			this->iRenderer->SetOldXRotation( this->iRenderer->GetXRotation() );
			this->iRenderer->SetOldYRotation( this->iRenderer->GetYRotation() );
			this->iRenderer->GetTrackball().MouseDown(Vector3<float>(x, y, 0));
		}
		else
		{
			iMouseButtonDown = EMouseUp;
			this->iRenderer->GetTrackball().MouseUp(Vector3<float>(x, y, 0));
		}
		break;
		//Set mesh position
	case GLUT_RIGHT_BUTTON:
		if(GLUT_UP == state)
		{
			iMouseButtonDown = EMouseUp;
		}
		else
		{
			iMouseButtonDown = EMouseDownRight;
		}
		break;
	default:
		break;
	}

}

void UIHandler::ProcessMouseMotionEvent( int x, int y )
{
	if( EMouseDownLeft == iMouseButtonDown)
	{
		this->iRenderer->SetXRotation( this->iRenderer->GetOldXRotation() 
			+ (float)(y - this->iMouseY) / 4.0);
		this->iRenderer->SetYRotation( this->iRenderer->GetOldYRotation() 
			+ (float)(x - this->iMouseX) / 4.0);
		
		this->iRenderer->GetTrackball().MouseMove(Vector3<float>(x, y, 0));
	}
	else if( EMouseDownRight == iMouseButtonDown)
	{
		this->iRenderer->SetZoom(this->iRenderer->GetZoom()+(((float)y - (float)this->iMouseY) / this->iRenderer->GetScreenHeightInPixels()) * 20);
		this->iMouseY = y;
	}
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

