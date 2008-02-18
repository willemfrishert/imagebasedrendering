#include "Vector3.h"
#include "Matrix4.h"
#include "Quaternion.h"

Quaternion::Quaternion(void)
{
}

Quaternion::Quaternion(const float _scalar, const Vector3<float> _vector):
scalar(_scalar), vector(_vector)
{
}

Quaternion::~Quaternion(void)
{}

const float& Quaternion::Scalar() const
{
	return scalar;
}

const Vector3<float>& Quaternion::Vector() const
{
	return vector;
}

float& Quaternion::Scalar()
{
	return scalar;
}

Vector3<float>& Quaternion::Vector()
{
	return vector;
}

void Quaternion::ToMatrix( Matrix4& rotMatrix ) const
{
	float x = vector.x();
	float y = vector.y();
	float z = vector.z();
	float yy = y * y;
	float zz = z * z;
	float xx = x * x;
	float w = this->scalar;

	rotMatrix.a11 = 1 - 2 * (yy + zz);
	rotMatrix.a12 = 2 * (x * y - z * w);
	rotMatrix.a13 = 2 * (z * x + y * w);
	rotMatrix.a14 = 0;

	rotMatrix.a21 = 2 * (x * y + z * w);
	rotMatrix.a22 = 1 - 2 * (zz + xx);
	rotMatrix.a23 = 2 * (y * z - x * w);
	rotMatrix.a24 = 0;

	rotMatrix.a31 = 2 * (z * x - y * w);
	rotMatrix.a32 = 2 * (y * z + x * w);
	rotMatrix.a33 = 1 - 2 * (yy + xx);
	rotMatrix.a34 = 0;

	rotMatrix.a41 = 0;
	rotMatrix.a42 = 0;
	rotMatrix.a43 = 0;
	rotMatrix.a44 = 1;
}

void Quaternion::LoadIdentity()
{
	scalar = 1;
	vector.setX( 0 );
	vector.setY( 0 );
	vector.setZ( 0 );
}

Quaternion& Quaternion::operator *= ( Quaternion const & other)
{
	// w = w*w' - v.v'
	scalar = scalar * other.Scalar() - ( vector * other.Vector() );

	// v = v^v' + w.v' + w'.v
	vector = ( vector ^ other.Vector() ) + ( other.Vector() * scalar )
		+ ( vector * other.Scalar() );

	return *this;
}

Quaternion Quaternion::operator * ( Quaternion const & other) const
{
	Quaternion result;

	// w = w*w' - v.v'
	result.scalar = scalar * other.Scalar() - ( vector * other.Vector() );

	// v = v^v' + w.v' + w'.v
	result.vector = ( vector ^ other.Vector() ) + ( other.Vector() * scalar )
		+ ( vector * other.Scalar() );
	
	return result;
}

void Quaternion::ComputeQuaternion(const float angle, const Vector3<float>& axis)
{
	vector = axis;
	vector.Normalize();

	vector *= sin(angle / static_cast<float>(2));
	scalar = cos(angle / static_cast<float>(2));
}