#pragma once

#include <string>
#include <vector>

#include "GL/glut.h"

using namespace std;

// GLUT_BITMAP_8_BY_13,
// GLUT_BITMAP_9_BY_15,
// GLUT_BITMAP_TIMES_ROMAN_10,
// GLUT_BITMAP_TIMES_ROMAN_24,
// GLUT_BITMAP_HELVETICA_10,
// GLUT_BITMAP_HELVETICA_12,
// GLUT_BITMAP_HELVETICA_18,


class GLMenu
{
public:
	GLMenu(int aWindowHeight, int aWindowWidth, int aGlutFont);
	~GLMenu(void);

	void addMenuLine(string line);

	void draw();

	//attributes
private:
	std::vector<char*> lines;
	int iWindowHeight;
	int iWindowWidth;
	int iGlutFont;
};
