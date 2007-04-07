

#include "Matrix4.h"
#include <math.h>


Matrix4 Matrix4::defaultMatrix;

/** \brief Default constructor
*
*
*	     The default constructor create a identity matrix
*
*		 1     0     0     0   <br>
*		 0     1     0     0   <br>
*		 0     0     1     0   <br>
*		 0     0     0     1   <br>
*
*
*/
Matrix4::Matrix4(){

	this->a11 = 1;
	this->a12 = 0;
	this->a13 = 0;
	this->a14 = 0;
	
	this->a21 = 0;
	this->a22 = 1;
	this->a23 = 0;
	this->a24 = 0;
	
	this->a31 = 0;
	this->a32 = 0;
	this->a33 = 1;
	this->a34 = 0;
	
	this->a41 = 0;
	this->a42 = 0;
	this->a43 = 0;
	this->a44 = 1;
}

/** \brief Copy constructor
*
*	Copy the Matrix4 object that is given as argument
*
* \param _v the object that shall be copied
*/
Matrix4::Matrix4(const Matrix4& _v){


	this->a11 = _v.a11;
	this->a12 = _v.a12;
	this->a13 = _v.a13;
	this->a14 = _v.a14;
	
	this->a21 = _v.a21;
	this->a22 = _v.a22;
	this->a23 = _v.a23;
	this->a24 = _v.a24;
	
	this->a31 = _v.a31;
	this->a32 = _v.a32;
	this->a33 = _v.a33;
	this->a34 = _v.a34;
	
	this->a41 = _v.a41;
	this->a42 = _v.a42;
	this->a43 = _v.a43;
	this->a44 = _v.a44;

}

/** \brief Set constructor
*
*
*	     The matrix is build up as following
*
*		 a11     a12     a13     a14   <br>
*		 a21     a22     a23     a24   <br>
*		 a31     a32     a33     a34   <br>
*		 a41     a42     a43     a44   <br>
*
*
*/
Matrix4::Matrix4(float _a11, float _a12, float _a13, float _a14,
		 float _a21, float _a22, float _a23, float _a24,
		 float _a31, float _a32, float _a33, float _a34,
		 float _a41, float _a42, float _a43, float _a44){
		

	this->a11 = _a11;
	this->a12 = _a12;
	this->a13 = _a13;
	this->a14 = _a14;
	
	this->a21 = _a21;
	this->a22 = _a22;
	this->a23 = _a23;
	this->a24 = _a24;
	
	this->a31 = _a31;
	this->a32 = _a32;
	this->a33 = _a33;
	this->a34 = _a34;
	
	this->a41 = _a41;
	this->a42 = _a42;
	this->a43 = _a43;
	this->a44 = _a44;	
		
}

// **** Public Functions

/** \brief Clear the matrix
*
*
* Build up an indentity matrix
*
*		 1     0     0     0   <br>
*		 0     1     0     0   <br>
*		 0     0     1     0   <br>
*		 0     0     0     1   <br>
*
*
*/
void Matrix4::clear(){

	this->a11 = 1;
	this->a12 = 0;
	this->a13 = 0;
	this->a14 = 0;
	
	this->a21 = 0;
	this->a22 = 1;
	this->a23 = 0;
	this->a24 = 0;
	
	this->a31 = 0;
	this->a32 = 0;
	this->a33 = 1;
	this->a34 = 0;
	
	this->a41 = 0;
	this->a42 = 0;
	this->a43 = 0;
	this->a44 = 1;

}

/** \brief Method that create a transformation matrix for rotation around X
*
*	This method create an Matrix4 object, calculate an rotation
*	matrix given the input, and returns the transformation matrix.
*
*	\param _rx the rotation angle
*	\return Matrix4 that describe the rotation around X
*
*
*/
Matrix4 Matrix4::getRotateXInstance(float _rx){

	Matrix4 m = getDefaultMatrix();

	float cosine = cos( _rx );
	float sine = sin( _rx );
	//float cosine = MathTrig::cosine( _rx );
	//float sine = MathTrig::sine( _rx );
	
	m.a22 = m.a33 = cosine;
	m.a23 = sine * -1;
	m.a32 = sine;

	return m;
	
}

/** \brief Method that create a transformation matrix for rotation around Y
*
*	This method create an Matrix4 object, calculate an rotation
*	matrix given the input, and returns the transformation matrix.
*
*	\param _ry the rotation angle
*	\return Matrix4 that describe the rotation around Y
*
*
*/
Matrix4 Matrix4::getRotateYInstance(float _ry){

	Matrix4 m = getDefaultMatrix();
	
	float cosine = cos( _ry );
	float sine = sin( _ry );
	//float cosine = MathTrig::cosine( _ry );
	//float sine = MathTrig::sine( _ry );

	m.a11 = m.a33 = cosine;
	m.a13 = sine;
	m.a31 = sine * -1;

	
	return m;
}

/** \brief Method that create a transformation matrix for rotation around Z
*
*	This method create an Matrix4 object, calculate an rotation
*	matrix given the input, and returns the transformation matrix.
*
*	\param _rz the rotation angle
*	\return Matrix4 that describe the rotation around Z
*
*
*/
Matrix4 Matrix4::getRotateZInstance(float _rz){


	Matrix4 m = getDefaultMatrix();

	float cosine = cos( _rz );
	float sine = sin( _rz );
	//float cosine = MathTrig::cosine( _rz );
	//float sine = MathTrig::sine( _rz );

	m.a11 = m.a22 = cosine;
	m.a21 = sine;
	m.a12 = sine * -1;
	
	return m;
}

/** \brief Method that create a transformation matrix for translation
*
*	Method that create an Matrix4 object, calculate an translation
*	matrix for translation in x,y,z direction, and return the Matrix4 object.
*
*	\param _tx translation along the x dimension
*	\param _ty translation along the y dimension
*	\param _tz translation along the z dimension
*
*	\return Matrix4 that describe the translation
*
*
*/
Matrix4 Matrix4::getTranslateInstance(float _tx, float _ty, float _tz){

	Matrix4 m = getDefaultMatrix();
	
		m.a14 = _tx;
		m.a24 = _ty;
		m.a34 = _tz;
	
	return m;
}

Matrix4 Matrix4::getTranslateInstance(const Vector3<float>& v) {
	
	Matrix4 m = getDefaultMatrix();

	m.a14 = v.x();
	m.a24 = v.y();
	m.a34 = v.z();

	return m;
}


/** \brief Method that create a matrix for uniform scaling
*
*	Method that create an Matrix4 object, calculate an uniform
*	scaling matrix, and return the Matrix4 object.
*
*	\param  _s The scaling factor
*
*	\return Matrix4 that describe the uniform scaling
*
*
*/
Matrix4 Matrix4::getScaleInstance(float _s){
	
	Matrix4 m = getDefaultMatrix();
	
	m.a11 = m.a22 = m.a33 = _s;
	
	return m;

}

/** \brief Method that create a matrix for non-uniform scaling
*
*	Method that create an Matrix4 object, calculate an non-uniform
*	scaling matrix, and return the Matrix4 object.
*
*	\param  _sx The scaling factor along the x dimension
*	\param  _sy The scaling factor along the y dimension
*	\param  _sz The scaling factor along the z dimension
*
*	\return Matrix4 that describe the non-uniform scaling
*
*
*/
Matrix4 Matrix4::getScaleInstance(float _sx, float _sy, float _sz){

	Matrix4 m = getDefaultMatrix();

	m.a11 = _sx;
	m.a22 = _sy;
	m.a33 = _sz;
	
	return m;

}

/** 
 * \brief Method that copy the value from another matrix
 *
 */
void Matrix4::set(Matrix4 _m4){

	
	this->a11 = _m4.a11;
	this->a12 = _m4.a12;
	this->a13 = _m4.a13;
	this->a14 = _m4.a14;
	
	this->a21 = _m4.a21;
	this->a22 = _m4.a22;
	this->a23 = _m4.a23;
	this->a24 = _m4.a24;
	
	this->a31 = _m4.a31;
	this->a32 = _m4.a32;
	this->a33 = _m4.a33;
	this->a34 = _m4.a34;
	
	this->a41 = _m4.a41;
	this->a42 = _m4.a42;
	this->a43 = _m4.a43;
	this->a44 = _m4.a44;
	
}

/** \brief Method to concatenate (rith) with another matrix
*
* Method to concatenate (rith) with another matrix
*
* \param _m4 the matrix to concatenate from the right with this matrix
*
*/
void Matrix4::RightConcat(Matrix4 _m4){

	this->set(this->mult(_m4));


}

/** \brief Method to concatenate (left) with another matrix
*
* Method to concatenate (left) with another matrix
*
* \param _m4 the matrix to concatenate from the left with this matrix
*
*/
void Matrix4::LeftConcat(Matrix4 _m4){

	this->set(_m4.mult(*this));
}

/** \brief Method to multiply this with another matrix
*
* Method to multiply this with another matrix
*
* \param _m4 the second (right) matrix for the multiplication
* \return The resulting matrix product
*
*/
Matrix4 Matrix4::mult(const Matrix4& _m4){

	Matrix4 m = getDefaultMatrix();

	// 1th row
	m.a11 = this->a11 * _m4.a11 + this->a12 * _m4.a21 + this->a13 * _m4.a31 + this->a14 * _m4.a41;
	m.a12 = this->a11 * _m4.a12 + this->a12 * _m4.a22 + this->a13 * _m4.a32 + this->a14 * _m4.a42;
	m.a13 = this->a11 * _m4.a13 + this->a12 * _m4.a23 + this->a13 * _m4.a33 + this->a14 * _m4.a43;
	m.a14 = this->a11 * _m4.a14 + this->a12 * _m4.a24 + this->a13 * _m4.a34 + this->a14 * _m4.a44;

	// 2th row
	m.a21 = this->a21 * _m4.a11 + this->a22 * _m4.a21 + this->a23 * _m4.a31 + this->a24 * _m4.a41;
	m.a22 = this->a21 * _m4.a12 + this->a22 * _m4.a22 + this->a23 * _m4.a32 + this->a24 * _m4.a42;
	m.a23 = this->a21 * _m4.a13 + this->a22 * _m4.a23 + this->a23 * _m4.a33 + this->a24 * _m4.a43;
	m.a24 = this->a21 * _m4.a14 + this->a22 * _m4.a24 + this->a23 * _m4.a34 + this->a24 * _m4.a44;

	// 3th row
	m.a31 = this->a31 * _m4.a11 + this->a32 * _m4.a21 + this->a33 * _m4.a31 + this->a34 * _m4.a41;
	m.a32 = this->a31 * _m4.a12 + this->a32 * _m4.a22 + this->a33 * _m4.a32 + this->a34 * _m4.a42;
	m.a33 = this->a31 * _m4.a13 + this->a32 * _m4.a23 + this->a33 * _m4.a33 + this->a34 * _m4.a43;
	m.a34 = this->a31 * _m4.a14 + this->a32 * _m4.a24 + this->a33 * _m4.a34 + this->a34 * _m4.a44;

	// 4th row
	m.a41 = this->a41 * _m4.a11 + this->a42 * _m4.a21 + this->a43 * _m4.a31 + this->a44 * _m4.a41;
	m.a42 = this->a41 * _m4.a12 + this->a42 * _m4.a22 + this->a43 * _m4.a32 + this->a44 * _m4.a42;
	m.a43 = this->a41 * _m4.a13 + this->a42 * _m4.a23 + this->a43 * _m4.a33 + this->a44 * _m4.a43;
	m.a44 = this->a41 * _m4.a14 + this->a42 * _m4.a24 + this->a43 * _m4.a34 + this->a44 * _m4.a44;
	
	return m;
}


///** \brief Mulitply a matrix with a vertex to transform it.
// *
// *
// * This method takes a Vertex4f and multiply this with the Matrix4 object
// * and return the result as a Vertex4f.
// *
// *
// *\param _v4 the vertex that should be multiplied with the matrix4
// *\return Vertex4f with the result
// *
// */
//Vertex4f Matrix4::mult(const Vertex4f& _v4) const {
//
//	float x;
//	float y;
//	float z;
//	float w;
//
//	float vx = _v4.x();
//	float vy = _v4.y();
//	float vz = _v4.z();
//	float vw = _v4.getW();
//
//	x = this->a11 * vx + this->a12 * vy + this->a13 * vz + this->a14 * vw;
//	y = this->a21 * vx + this->a22 * vy + this->a23 * vz + this->a24 * vw;
//	z = this->a31 * vx + this->a32 * vy + this->a33 * vz + this->a34 * vw;
//	w = this->a41 * vx + this->a42 * vy + this->a43 * vz + this->a44 * vw;
//	
//	return Vertex4f(x, y, z, w);
//}

///**
// * XXX: I think it's done like mult. So it should be switched !!!!
// * @param 
// * @return 
// */
//Vertex4f Matrix4::preMult(Vertex4f _v4){
//
//	float x;
//	float y;
//	float z;
//	float w;
//
//	//float vx = _v4.x();
//	//float vy = _v4.y();
//	//float vz = _v4.z();
//	//float vw = _v4.getW();
//
//	x = this->a11 * _v4.x() + this->a21 * _v4.y() + this->a31 * _v4.z() + this->a41 * _v4.getW();
//	y = this->a12 * _v4.x() + this->a22 * _v4.y() + this->a32 * _v4.z() + this->a42 * _v4.getW();
//	z = this->a13 * _v4.x() + this->a23 * _v4.y() + this->a33 * _v4.z() + this->a43 * _v4.getW();
//	w = this->a14 * _v4.x() + this->a24 * _v4.y() + this->a34 * _v4.z() + this->a44 * _v4.getW();
//
//	return Vertex4f(x, y, z, w);
//}

/** \brief Multiply matrix with a 3-element vector to transform it
 *
 * This method transform a Vector3<float> with the matrix object. Only
 * the upper left 3x3 portion of the matrix should be used.
 *
 *	(Translation and homogeneous coordinate changes are ignored)
 *
 * \param _v3 The vector to transform
 * \return Vector3<float> containing the transformed vertex
 *
 */
Vector3<float> Matrix4::mult(Vector3<float> _v3) const {
	
	float x;
	float y;
	float z;

	float vx = _v3.x();
	float vy = _v3.y();
	float vz = _v3.z();

	x = this->a11 * vx + this->a12 * vy + this->a13 * vz;
	y = this->a21 * vx + this->a22 * vy + this->a23 * vz;
	z = this->a31 * vx + this->a32 * vy + this->a33 * vz;

	return Vector3<float>(x, y, z);
	
}

/**
 * @return the transpose of the current matrix
 */
Matrix4 Matrix4::transpose() const
{
	return Matrix4(this->a11, this->a21, this->a31, this->a41,
				   this->a12, this->a22, this->a32, this->a42,
				   this->a13, this->a23, this->a33, this->a43,
				   this->a14, this->a24, this->a34, this->a44);
}

/** \brief Multiply two matrices
 *
 *	Method that multiply two matrices and return the result
 * 
 *
 *\param  _m1 the first (left) matrix for the multiplication
 *\param  _m2 the second (right) matrix for the multiplication
 *\return Matrix4 the resulting matrix product
 *
 */
Matrix4 Matrix4::mult(Matrix4 _m1, Matrix4 _m2){

	return _m1.mult( _m2);
}

/**
 * @param m
 * @return 
 */
Matrix4 Matrix4::inverse(const Matrix4& m)
{
	// created as the identity by the default constructor
	Matrix4 inv;

	if (m.a11 == 0)
	{
		return inv;
	}

	float A[4][4] = {0};
	float I[4][4] = {0};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	A[0][0] = m.a11;
	A[0][1] = m.a12;
	A[0][2] = m.a13;
	A[0][3] = m.a14;

	A[1][0] = m.a21;
	A[1][1] = m.a22;
	A[1][2] = m.a23;
	A[1][3] = m.a24;

	A[2][0] = m.a31;
	A[2][1] = m.a32;
	A[2][2] = m.a33;
	A[2][3] = m.a34;

	A[3][0] = m.a41;
	A[3][1] = m.a42;
	A[3][2] = m.a43;
	A[3][3] = m.a44;

	I[0][0] = I[1][1] = I[2][2] = I[3][3] = 1;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	float ratio;
	int n = 4;
	
	// Gauss Elimination
	for (int i = 0; i < 4; i++)
	{
		for (int j = i+1; j < n; j++)
		{
			if (A[i][i] == 0)
			{
				// do something error
				return inv;
			}
			ratio = A[j][i] / A[i][i];

			// apply the modification to the all line
			for (int k = 0; k < n; k++)
			{
				A[j][k] -= ratio * A[i][k];
				I[j][k] -= ratio * I[i][k];
			}
		}
	}
	
	// Backward substitution
	for (int i = 0; i < n; i++)
	{
		// last element of i-th column
		I[n-1][i] /= A[n-1][n-1];

		for (int r = n-2; r >= 0; r--)
		{
			float sum = 0;

			// f is the column and is used as a row for Inv 
			for (int f = r+1; f < n; f++)
			{
				sum += A[r][f] * I[f][i];
			}
			I[r][i] = (I[r][i] - sum) / A[r][r];
		}
	}

	inv.a11 = I[0][0];
	inv.a12 = I[0][1];
	inv.a13 = I[0][2];
	inv.a14 = I[0][3];

	inv.a21 = I[1][0];
	inv.a22 = I[1][1];
	inv.a23 = I[1][2];
	inv.a24 = I[1][3];

	inv.a31 = I[2][0];
	inv.a32 = I[2][1];
	inv.a33 = I[2][2];
	inv.a34 = I[2][3];

	inv.a41 = I[3][0];
	inv.a42 = I[3][1];
	inv.a43 = I[3][2];
	inv.a44 = I[3][3];

	return inv;
}

///**
// * Returns the rotation matrix that transforms the Vector (1,0,0)
// * into the given Vector3<float> v; NOTE: v should be normalized.
// *
// * @param v the vector to whom we wish to find the rotation matrix: normalized
// * @param glMatrix the returned openGL format matrix
// */
//void Matrix4::rotationFromXAxis(const Vector3<float>& v, float* glMatrix)
//{
//	Matrix4 m;
//
//	// angle between v and the x-axis
//	float alphaX = v.dot(Vector3<float>(1, 0, 0));
//
//	// angle between v and the y-axis
//	float alphaY = v.dot(Vector3<float>(0, 1, 0));
//
//	// angle between v and the z-axis
//	float alphaZ = v.dot(Vector3<float>(0, 0, 1));
//
//	float d = sqrt(alphaY*alphaY + alphaZ*alphaZ);
//
//	// rotate around the x-axis
//	m.a22 = alphaZ / d;
//	m.a23 = alphaY / d;
//	m.a32 = -alphaY / d;
//	m.a33 = alphaZ / d;
//
//	// rotate around the y-axis
//	Matrix4 Ry;
//
//	Ry.a11 = alphaX;
//	Ry.a31 = d;
//	Ry.a13 = -d;
//	Ry.a33 = alphaX;
//
//	m = m.mult( Ry );
//
//	glMatrix[0] = m.a11; glMatrix[4] = m.a12; glMatrix[8] = m.a13; glMatrix[12] = m.a14;
//	glMatrix[1] = m.a21; glMatrix[5] = m.a22; glMatrix[9] = m.a23; glMatrix[13] = m.a24;
//	glMatrix[2] = m.a31; glMatrix[6] = m.a32; glMatrix[10] = m.a33; glMatrix[14] = m.a34;
//	glMatrix[3] = m.a41; glMatrix[7] = m.a42; glMatrix[11] = m.a43; glMatrix[15] = m.a44;
//}

/**
* Returns the rotation matrix that transforms the Vector (1,0,0)
* into the given Vector3<float> v; NOTE: v should be normalized.
*
* @param v the vector to whom we wish to find the rotation matrix: normalized
* @param glMatrix the returned openGL format matrix
*/
//void Matrix4::rotationToXAxis(const Vector3<float>& v, float* glMatrix)
//{
//	Matrix4 m = rotationToXAxis( v );
//
//	m = Matrix4::inverse( m );
//
//	glMatrix[0] = m.a11; glMatrix[4] = m.a12; glMatrix[8] = m.a13; glMatrix[12] = m.a14;
//	glMatrix[1] = m.a21; glMatrix[5] = m.a22; glMatrix[9] = m.a23; glMatrix[13] = m.a24;
//	glMatrix[2] = m.a31; glMatrix[6] = m.a32; glMatrix[10] = m.a33; glMatrix[14] = m.a34;
//	glMatrix[3] = m.a41; glMatrix[7] = m.a42; glMatrix[11] = m.a43; glMatrix[15] = m.a44;
//}

void Matrix4::rotationToAndFromXAxis(const Vector3<float>& v, float* fromGLMatrix, float* toGLMatrix)
{
	Matrix4 from, to;

	rotationToAndFromXAxis(v, to, from);

	fromGLMatrix[0] = from.a11; fromGLMatrix[4] = from.a12; fromGLMatrix[8] = from.a13; 
	fromGLMatrix[12] = from.a14;
	fromGLMatrix[1] = from.a21; fromGLMatrix[5] = from.a22; fromGLMatrix[9] = from.a23; 
	fromGLMatrix[13] = from.a24;
	fromGLMatrix[2] = from.a31; fromGLMatrix[6] = from.a32; fromGLMatrix[10] = from.a33; 
	fromGLMatrix[14] = from.a34;
	fromGLMatrix[3] = from.a41; fromGLMatrix[7] = from.a42; fromGLMatrix[11] = from.a43; 
	fromGLMatrix[15] = from.a44;

	toGLMatrix[0] = to.a11; toGLMatrix[4] = to.a12; toGLMatrix[8] = to.a13; 
	toGLMatrix[12] = to.a14;
	toGLMatrix[1] = to.a21; toGLMatrix[5] = to.a22; toGLMatrix[9] = to.a23; 
	toGLMatrix[13] = to.a24;
	toGLMatrix[2] = to.a31; toGLMatrix[6] = to.a32; toGLMatrix[10] = to.a33; 
	toGLMatrix[14] = to.a34;
	toGLMatrix[3] = to.a41; toGLMatrix[7] = to.a42; toGLMatrix[11] = to.a43; 
	toGLMatrix[15] = to.a44;
}

//Matrix4 Matrix4::rotationToXAxis(Vector3<float> v)
//{
//	Matrix4 Rz;
//
//	// angle between v and the x-axis
//	float alphaX = v.dot(Vector3<float>(1, 0, 0));
//
//	// angle between v and the y-axis
//	float alphaY = v.dot(Vector3<float>(0, 1, 0));
//
//	// angle between v and the z-axis
//	float alphaZ = v.dot(Vector3<float>(0, 0, 1));
//
//	float d = sqrt(alphaY*alphaY + alphaX*alphaX);
//
//	// rotate around the y-axis
//	Matrix4 Ry;
//
//	if ( alphaZ != 0)
//	{
//		Ry.a11 = d;
//		Ry.a31 = -alphaZ;
//		Ry.a13 = alphaZ;
//		Ry.a33 = d;
//	}
//
//	if ( d != 0)
//	{
//		// rotate around the z-axis
//		Rz.a11 = alphaX / d;
//		Rz.a12 = alphaY / d;
//		Rz.a21 = -alphaY / d;
//		Rz.a22 = alphaX / d;
//
//		return Ry.mult( Rz );
//	}
//
//	return Ry;
//}

void Matrix4::rotationToAndFromXAxis(Vector3<float> v, Matrix4& to, Matrix4& from)
{
	Matrix4 Rz;

	// angle between v and the x-axis
	float alphaX = v * Vector3<float>(1, 0, 0);

	// angle between v and the y-axis
	float alphaY = v * Vector3<float>(0, 1, 0);

	// angle between v and the z-axis
	float alphaZ = v * Vector3<float>(0, 0, 1);

	float d = sqrt(alphaY*alphaY + alphaX*alphaX);

	// rotate around the y-axis
	Matrix4 Ry;

	if ( alphaZ != 0)
	{
		to.a11 = d;
		to.a31 = -alphaZ;
		to.a13 = alphaZ;
		to.a33 = d;

		// just negate the sin values
		from.a11 = d;
		from.a31 = alphaZ;
		from.a13 = -alphaZ;
		from.a33 = d;
	}

	if ( d != 0)
	{
		// rotate around the z-axis
		Rz.a11 = alphaX / d;
		Rz.a12 = alphaY / d;
		Rz.a21 = -alphaY / d;
		Rz.a22 = alphaX / d;

		to = to.mult( Rz );

		// the same here: negate the sines
		Rz.a11 = alphaX / d;
		Rz.a12 = -alphaY / d;
		Rz.a21 = alphaY / d;
		Rz.a22 = alphaX / d;

		from = Rz.mult( from );
	}
}

/**
 * @param axis the axis of rotation
 * @param p the point where the line starts
 * @param angle specified in radians
 * @return 
 */
Matrix4 Matrix4::rotateAroundPointAndAxis(const Vector3<float>& axis, const Vector3<float>& p, float angle)
{
	Matrix4 from, to;

	rotationToAndFromXAxis(axis, to, from);

	/************************************************************************/
	/* EXPENSIVE USING SINE AND COSINE !!!!                                 */
	/************************************************************************/
	Matrix4 Rx = getRotateXInstance( angle );

	Matrix4 T = getTranslateInstance( p );
	Matrix4 invT = getTranslateInstance( p * (-1) );

	// T(p).Rz(-angle).Ry(-angle).Rx(angle).Ry(angle).Rz(angle).T(-p)
	return (((T.mult( from )).mult( Rx )).mult( to )).mult( invT );
}

void Matrix4::getGLMatrix(float* mat)
{
	mat[0] = this->a11; mat[4] = this->a12; mat[8] = this->a13; 
	mat[12] = this->a14;
	mat[1] = this->a21; mat[5] = this->a22; mat[9] = this->a23; 
	mat[13] = this->a24;
	mat[2] = this->a31; mat[6] = this->a32; mat[10] = this->a33; 
	mat[14] = this->a34;
	mat[3] = this->a41; mat[7] = this->a42; mat[11] = this->a43; 
	mat[15] = this->a44;
}

void Matrix4::loadIdentity()
{
	this->a11 = 1;
	this->a12 = 0;
	this->a13 = 0;
	this->a14 = 0;

	this->a21 = 0;
	this->a22 = 1;
	this->a23 = 0;
	this->a24 = 0;

	this->a31 = 0;
	this->a32 = 0;
	this->a33 = 1;
	this->a34 = 0;

	this->a41 = 0;
	this->a42 = 0;
	this->a43 = 0;
	this->a44 = 1;
}

Matrix4::~Matrix4(void){
	
}