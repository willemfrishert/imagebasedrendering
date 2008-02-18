#pragma once

#include <math.h>

template <class T> class Vector3 
{
public:
	Vector3(void);
	Vector3(const T& x, const T& y, const T& z);
	Vector3(const Vector3& other);
	Vector3 & operator = ( Vector3 const & other);
	~Vector3(void);

	Vector3 operator + ( Vector3 const & other) const;
	Vector3 operator - ( Vector3 const & other) const;
	Vector3 operator - () const;
	Vector3 operator * ( T const & other) const;
	Vector3 operator / ( Vector3 const & other) const;

	// Cross product
	Vector3 operator ^ ( const Vector3 & p ) const;
	// Dot product
	T operator * ( Vector3 const & other) const;

	Vector3 & operator += ( Vector3 const & other);
	Vector3 & operator *= ( T const & other);
	Vector3 & operator /= ( T const & other);
	Vector3 & operator -= ( Vector3 const & other);

	bool operator == ( Vector3 const & other) const;
	bool operator != ( Vector3 const & other) const;

	// Members Modifiers
	T &x();
	T &y();
	T &z();

	// Setup by polar coords
	void setByPolar( T ro, T theta, T fi );

	// Accesors
	const T &x() const;
	const T &y() const;
	const T &z() const;

	// Point's polar coords
	void Polar( T & ro, T & theta, T & fi ) const;

	T Norm() const;

	Vector3& Normalize();

	static T Angle( const Vector3& p1, const Vector3& p2);
	// Angle between unitary vectors
	static T AngleN( const Vector3& p1, const Vector3& p2);

	// Modifiers
	void add( const T& x, const T& y, const T& z );
	void setX(const T x);
	void setY(const T y);
	void setZ(const T z);
	void set(const T& x, const T& y, const T& z);
protected:
	T iX;
	T iY;
	T iZ;
}; // end class

template <class T>
inline 
Vector3<T>::Vector3():
iX(0), iY(0), iZ(0)
{
}

template <class T>
inline 
Vector3<T>::Vector3(const T& x, const T& y, const T& z):
iX(x), iY(y), iZ(z)
{
}

template <class T>
inline 
Vector3<T>::Vector3(const Vector3& other)
{
	iX = other.iX;
	iY = other.iY;
	iZ = other.iZ;
}

template <class T>
inline
Vector3<T> & Vector3<T>::operator = ( Vector3 const & other)
{
	iX = other.iX;
	iY = other.iY;
	iZ = other.iZ;

	return *this;
}

template <class T>
inline 
Vector3<T>::~Vector3() {}

template <class T>
inline 
Vector3<T> Vector3<T>::operator - ( Vector3 const & other) const
{ 
	return Vector3( iX - other.iX, iY - other.iY, iZ - other.iZ );
}

template <class T>
inline 
Vector3<T> Vector3<T>::operator - () const
{ 
	return Vector3( -iX, -iY, -iZ );
}

template <class T>
inline 
Vector3<T> Vector3<T>::operator + ( Vector3 const & other) const
{ 
	return Vector3( iX + other.iX, iY + other.iY, iZ + other.iZ );
}

template <class T>
inline 
Vector3<T> Vector3<T>::operator * ( T const & other	) const
{ 
	return Vector3( iX * other, iY * other, iZ * other );
}

template <class T>
inline 
Vector3<T> Vector3<T>::operator / ( Vector3 const & other) const
{ 
	return Vector3( iX / other.iX, iY / other.iY, iZ / other.iZ );
}

template <class T>
inline 
Vector3<T>& Vector3<T>::operator += ( Vector3 const & other)
{
	iX += other.iX;
	iY += other.iY;
	iZ += other.iZ;
	return *this;
}

template <class T>
inline 
Vector3<T>& Vector3<T>::operator -= ( Vector3 const & other)
{
	iX -= other.iX;
	iY -= other.iY;
	iZ -= other.iZ;
	return *this;
}

template <class T>
inline 
Vector3<T>& Vector3<T>::operator *= ( T const & other)
{
	iX *= other;
	iY *= other;
	iZ *= other;
	return *this;
}

template <class T>
inline 
Vector3<T>& Vector3<T>::operator /= ( T const & other)
{
	iX -= other;
	iY -= other;
	iZ -= other;
	return *this;
}

template <class T>
inline
bool Vector3<T>::operator == ( Vector3 const & other) const
{
	return ( (iX == other.iX) && (iY == other.iY) && (iZ == other.iZ) );
}

template <class T>
inline
bool Vector3<T>::operator != ( Vector3 const & other) const
{
	return ( (iX != other.iX) && (iY != other.iY) && (iZ != other.iZ) );
}

template <class T>
inline
const T& Vector3<T>::x() const
{ return iX; }

template <class T>
inline
const T& Vector3<T>::y() const
{ return iY; }

template <class T>
inline
const T& Vector3<T>::z() const
{ return iZ; }

template <class T>
inline
T& Vector3<T>::x()
{ return iX; }

template <class T>
inline
T& Vector3<T>::y()
{ return iY; }

template <class T>
inline
T& Vector3<T>::z()
{ return iZ; }

template <class T>
inline
void Vector3<T>::Polar( T & ro, T & theta, T & fi ) const
{
	ro = Norm();
	theta = (T)atan2( iY, iX );
	fi    = (T)acos( iZ / ro );
}

template <class T>
inline
void Vector3<T>::setByPolar( T ro, T theta, T fi )
{
	iX = ro * sin(fi) * cos(theta);
	iY = ro * sin(theta) * sin(fi);
	iZ = ro * cos(fi);

	//ro = sqrt(x*x + y*y + z*z) 
	//fi = atan(y / x) 
	//theta	= asin(sqrt(x*x + y*y) / r) = acos(z / r) 

	//x = ro sin(fi) cos(theta) 
	//y = ro sin(theta) sin(fi) 
	//z = ro cos(fi) 
}

template <class T>
inline
Vector3<T>& Vector3<T>::Normalize()
{
	T norm = this->Norm();
	if( norm > 0.0 )
	{
		iX /= norm;
		iY /= norm;
		iZ /= norm;
	}

	return *this;
}

template <class T>
inline
Vector3<T> Vector3<T>::operator ^ ( const Vector3 & p ) const
{
	return Vector3
		(
		(iY * p.iZ) - (iZ * p.iY),
		(iZ * p.iX) - (iX * p.iZ),
		(iX * p.iY) - (iY * p.iX)
		);
}

template <class T>
inline
T Vector3<T>::operator * (const Vector3 & p) const
{
	return ( (iX * p.iX) + (iY * p.iY) + (iZ * p.iZ) );
}

template <class T>
inline
T Vector3<T>::Norm() const
{
	return sqrt( iX * iX + iY * iY + iZ * iZ );
}

template <class T>
inline
T Vector3<T>::Angle( const Vector3& p1, const Vector3& p2)
{
	T w = p1.Norm() * p2.Norm();

	if( w == 0 ) 
	{
		return -1;
	}

	T t = ( p1 * p2 ) / w;

	if( t > 1) 
	{
		t = 1;
	}
	else 
	{
		if( t < -1 )
		{
			t = -1;
		}
	}

	return (T) acos(t);
}

template <class T>
inline
T Vector3<T>::AngleN( const Vector3& p1, const Vector3& p2)
{
	T w = p1 * p2;
	if( w > 1 ) 
	{
		w = 1;
	}
	else
	{
		if( w < -1 ) 
		{
			w =- 1;
		}
	}

	return (T) acos(w);
}

template <class T>
inline 
void Vector3<T>::add( const T& x, const T& y, const T& z )
{
	iX += x;
	iY += y;
	iZ += z;
}

template <class T>
inline
void Vector3<T>::setX(const T x)
{
	iX = x;
}

template <class T>
inline
void Vector3<T>::setY(const T y)
{
	iY = y;
}

template <class T>
inline
void Vector3<T>::setZ(const T z)
{
	iZ = z;
}

template <class T>
inline
void Vector3<T>::set(const T& x, const T& y, const T& z)
{
	iX = x;
	iY = y;
	iZ = z;
}