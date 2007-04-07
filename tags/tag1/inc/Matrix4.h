#ifndef MATRIX4F1_H
#define MATRIX4F1_H

//#include "Vertex4f.h"
#include "Vector3.h"


/** \brief Class that define an 4x4 matrix
*
*	Class that define an 4x4 matrix, different mathematical
*	operations that is associated with matrix and
*	some 3D computer graphics methods that uses matrix
*
*
*	The matrix is build up as following
*
*		 a11     a12     a13     a14   <br>
*		 a21     a22     a23     a24   <br>
*		 a31     a32     a33     a34   <br>
*		 a41     a42     a43     a44   <br>

*/

class Matrix4 {

 private:

	 /**
	  * \brief used by static methods
	  *
	  * It's only created once
	  */
	 static Matrix4 defaultMatrix;

	 inline static Matrix4& getDefaultMatrix() { defaultMatrix.clear(); return defaultMatrix; };
 public:
 
/** \brief Public variables, check the details what corresponds to what
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
 	float a11, a12, a13, a14,a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44;
	
	// Constructors    	
	Matrix4();
	
	Matrix4(const Matrix4&);
	
	Matrix4(float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float);
	
	// functions
	void clear(void);

	static Matrix4 getRotateXInstance(float);
	
	static Matrix4 getRotateYInstance(float);
	
	static Matrix4 getRotateZInstance(float);
	
	static Matrix4 getTranslateInstance(float, float, float);

	static Matrix4 getTranslateInstance(const Vector3<float>&);
	
	static Matrix4 getScaleInstance(float);
	
	static Matrix4 getScaleInstance(float, float, float);

	//static void rotationToXAxis(const Vector3<float>& v, float* glMatrix);

	static void rotationToAndFromXAxis(const Vector3<float>& v, float* fromGLMatrix, float* toGLMatrix);

	//static Matrix4 rotationToXAxis(Vector3<float> v);

	static void rotationToAndFromXAxis(Vector3<float> v, Matrix4& to, Matrix4& from);

	static Matrix4 rotateAroundPointAndAxis(const Vector3<float>& axis, const Vector3<float>& p, float angle);

	void set(Matrix4); 	
	
	void RightConcat(Matrix4);
	
	void LeftConcat(Matrix4);
	
	Matrix4 mult(const Matrix4&);
	
	//Vertex4f mult(const Vertex4f&) const;
	
	//Vertex4f preMult(Vertex4f);

	Matrix4 transpose() const;
	
	Vector3<float> mult(Vector3<float>) const;
	
	static Matrix4 mult(Matrix4, Matrix4);

	static Matrix4 inverse(const Matrix4& m);

	void getGLMatrix(float* mat);

	void loadIdentity();
	
	~Matrix4(void); 
};

#endif
