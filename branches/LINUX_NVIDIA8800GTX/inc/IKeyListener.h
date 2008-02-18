#pragma once

class IKeyListener
{
public:
  	virtual void ProcessNormalKeys( unsigned char key, int x, int y ) = 0;
	virtual void ProcessCursorKeys( int key, int x, int y ) = 0;
};
