#pragma once

#include "Basic.h"
#include "VirtualTrackball.h"

//CLASS DECLARATION

class Renderer;

/** \brief User interface class.
*
* This class is for functions of user interaction.
*/
class UIHandler
{
	//PUBLIC FUNCTIONS
public:
	//CONSTRUCTORS
	//CMyUiEvents();
	UIHandler(Renderer* aRenderer);

	//DESTRUCTOR
	~UIHandler();

	//MEMBER FUNCTIONS
	void ProcessNormalKeys( unsigned char key, int x, int y );
	void ProcessCursorKeys( int key, int x, int y );
	void ProcessMouseEvent( int button, int state, int x, int y );
	void ProcessMouseMotionEvent( int x, int y );

	//PUBLIC DATA
public:
	static UIHandler* iCurrentUi;

	//PRIVATE DATA
private:
	enum TMouseDownStatus
	{
		EMouseUp = 0,
		EMouseDownLeft,
		EMouseDownRight
	};

	Renderer* iRenderer;
	TMouseDownStatus iMouseButtonDown;
	int iMouseX;
	int iMouseY;	
};

extern void ProcessNormalKeysWithUi( unsigned char key, int x, int y );
extern void ProcessCursorKeysWithUi( int key, int x, int y );
extern void ProcessMouseEventWithUi( int button, int state, int x, int y);
extern void ProcessMouseMotionEventWithUi( int x, int y );