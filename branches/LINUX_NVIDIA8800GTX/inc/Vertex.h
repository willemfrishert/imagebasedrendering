/**
 * @description NOTE: the iNormal, iBinormal and iTangent can be used either
 * to store the normal, binormal and tangent, or the inverse normal, inverse binormal
 * and inverse tangent. This will, of course, depend on the mapping technique we're
 * using.
 * 
 * @file Vertex.h
 * @author João Pedro Jorge
 */
#pragma once

#include "Vector3.h"
#include "Vector2.h"

class Vertex
{
public:
	Vertex(void);
	Vertex(const Vector3<float>& aNormal, const Vector3<float>& aPosition, const Vector2<float>& aTexCoord);

	~Vertex(void);

	//PRIVATE DATA
public:
	Vector3<float> iPosition;

	Vector3<float> iNormal;

	Vector2<float> iTexCoord;
};
