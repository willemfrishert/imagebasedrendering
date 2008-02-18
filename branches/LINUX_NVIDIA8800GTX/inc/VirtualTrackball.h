#pragma once

class Quaternion;
class Matrix4;

class VirtualTrackball
{
public:
	VirtualTrackball(void);
	VirtualTrackball(int winWidth, int winHeight);
	~VirtualTrackball(void);

	void MouseDown(const Vector3<float>& startPoint);
	void MouseUp(const Vector3<float>& endPoint);
	void MouseMove(const Vector3<float>& curPoint);
	void DoRotation() const;
	void SetWindowRect(const int winWidth, const int winHeight);
	void SetResolution(const float resolution);
	//static void RotateMatrix(const float angle, float matrix[16]);

	void getRotationMatrix(float* mat);
	Matrix4 getRotationMatrix();
	inline void reset() { m_curQuat.LoadIdentity();
						m_lastQuat.LoadIdentity(); 
						m_rotMatrix.loadIdentity();};

protected:
	// methods
	Vector3<float> MapOnSphere(const Vector3<float>& mappingPoint, float radius);
	void MapCoords(float& x, float& y, const Vector3<float>& original);

protected:
	// atributes
	Vector3<float> m_LastMappedPoint;
	Matrix4 m_rotMatrix;

	Quaternion m_curQuat;
	Quaternion m_lastQuat;

	int m_iWidth;
	int m_iHeight;

	float m_dResolution;
};
