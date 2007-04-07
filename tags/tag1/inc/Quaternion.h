#pragma once

template <class T> class Vector3;
class Matrix4;

class Quaternion
{
public:
	Quaternion(void);
	Quaternion(const float _scalar, const Vector3<float> vector);
	~Quaternion(void);

	Quaternion& operator *= ( Quaternion const & other);
	Quaternion operator * ( Quaternion const & other) const;

	// Accessors
	const float& Scalar() const;
	const Vector3<float>& Vector() const;
	
	void ToMatrix( Matrix4& rotMatrix ) const;

	// Modifiers
	float& Scalar();
	Vector3<float>& Vector();

	void ComputeQuaternion(const float angle, const Vector3<float>& axis);
	
	void LoadIdentity();

protected:
	
	float scalar;
	Vector3<float> vector;

};







