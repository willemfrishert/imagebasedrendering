#pragma once

class IMouseListener
{
public:
	virtual void ProcessMouseMotionEvent( int x, int y ) = 0;
	virtual void ProcessMouseEvent( int button, int state, int x, int y ) = 0;
};
