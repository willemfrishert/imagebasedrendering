#include <cmath>
#include <iostream>

#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

#include "VirtualTrackball.h"
//#include "Utilities.h"

#define RADIUS 1

VirtualTrackball::VirtualTrackball(void)
{
	reset();
}

VirtualTrackball::VirtualTrackball(int winWidth, int winHeight):
m_iWidth(winWidth), m_iHeight(winHeight)
{
}

VirtualTrackball::~VirtualTrackball(void)
{
}

Vector3<float> VirtualTrackball::MapOnSphere(const Vector3<float>& mappingPoint, float radius)
{
	// Assuming a sphere of radius = 1
	Vector3<float> mapped(mappingPoint);

	float sqrX = mappingPoint.x() * mappingPoint.x();
	float sqrY = mappingPoint.y() * mappingPoint.y();

	//float z = sqrt( 1.0 - sqrX - sqrY );
	float z = sqrt( sqrX + sqrY );

	// Inside shpere
	if( z <  radius * 0.70710678118654752440) 
	{
		z = sqrt( (radius * radius) - ( z * z));
	}
	// Assume a hyperbolic sheet
	else
	{
		float t = radius / 1.41421356237309504880f;
		z = t*t / z;
	}

	mapped.setZ( z );

	return mapped;
}

void VirtualTrackball::MapCoords(float& x, float& y, const Vector3<float>& original)
{
	x = (2 * original.x() - m_iWidth) / m_iWidth;
	y = (m_iHeight - 2 * original.y()) / m_iHeight;
}

void VirtualTrackball::MouseDown(const Vector3<float>& startPoint)
{
	float mapCoordsX, mapCoordsY;
	MapCoords(mapCoordsX, mapCoordsY, startPoint);

	Vector3<float> normalizedPoint(mapCoordsX, mapCoordsY, 0);
	
	Vector3<float> curMappedPoint = MapOnSphere( normalizedPoint , RADIUS);

	m_LastMappedPoint = curMappedPoint;
}

void VirtualTrackball::MouseUp(const Vector3<float>& endPoint)
{
	float mapCoordsX, mapCoordsY;
	MapCoords(mapCoordsX, mapCoordsY, endPoint);

	Vector3<float> normalizedPoint(mapCoordsX, mapCoordsY, 0);

	Vector3<float> curMappedPoint = MapOnSphere( normalizedPoint , RADIUS);

	m_LastMappedPoint = curMappedPoint;
	
}

void VirtualTrackball::MouseMove(const Vector3<float>& curPoint)
{
	float mapCoordsX, mapCoordsY;
	MapCoords(mapCoordsX, mapCoordsY, curPoint);

	Vector3<float> normalizedPoint(mapCoordsX, mapCoordsY, 0);

	Vector3<float> curMappedPoint = MapOnSphere( normalizedPoint , RADIUS);

	// Cross product: find the axis of rotation
	Vector3<float> rotationAxis = m_LastMappedPoint ^ curMappedPoint;

	// Angle 
	float theta = rotationAxis.Norm() * m_dResolution;

	m_lastQuat.ComputeQuaternion(theta, rotationAxis);

	m_curQuat = m_lastQuat * m_curQuat;

	m_curQuat.ToMatrix(m_rotMatrix);

	m_LastMappedPoint = curMappedPoint;
}

void VirtualTrackball::getRotationMatrix(float* mat)
{
	return m_rotMatrix.getGLMatrix( mat );
}

Matrix4 VirtualTrackball::getRotationMatrix()
{
	return m_rotMatrix;
}

void VirtualTrackball::SetWindowRect(const int winWidth, const int winHeight)
{
	m_iWidth = winWidth;
	m_iHeight = winHeight;
}

void VirtualTrackball::SetResolution(const float resolution)
{
	m_dResolution = resolution;
}