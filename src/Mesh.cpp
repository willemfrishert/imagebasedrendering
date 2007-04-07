#include "ShaderAttributeObject.h"
#include "Mesh.h"

Mesh::Mesh()
: iIndicesCount( 0 )
, iVertexBinormalAttribObject( NULL )
, iVertexTangentAttribObject( NULL )
, iVertexTBNNormalAttribObject( NULL )
{
}

Mesh::~Mesh(void)
{
}

/**
 * @param aLocation
 * @description sets the binormal location on the shader
 */
void Mesh::setBinormalAttributeObject(ShaderAttributeObject* aBinormal)
{
	this->iVertexBinormalAttribObject = aBinormal;
}

/**
* @param aLocation
* @description sets the binormal location on the shader
*/
void Mesh::setTBNNormalAttributeObject(ShaderAttributeObject* aTBNNormal)
{
	this->iVertexTBNNormalAttribObject = aTBNNormal;
}

/**
 * @param aLocation
 * @description sets the tangent location on the shader
 */
void Mesh::setTangentAttributeObject(ShaderAttributeObject* aTangent)
{
	this->iVertexTangentAttribObject = aTangent;
}

/**
 *
 */
void Mesh::draw()
{
#ifndef GRAPHICS_DEBUG
	// Enable a vertex position, normal and texture coords arrays, respectively
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );  
	
	assert( this->iVertexTangentAttribObject );
	assert( this->iVertexTBNNormalAttribObject );
	assert( this->iVertexBinormalAttribObject );

	// Enable special attributes: Tangent and Binormal arrays
	glEnableVertexAttribArray( this->iVertexTangentAttribObject->getIndex() );
	glEnableVertexAttribArray( this->iVertexTBNNormalAttribObject->getIndex() );
	glEnableVertexAttribArray( this->iVertexBinormalAttribObject->getIndex() );
	
	// Passing the information to openGL using pointers to the actual arrays
	glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iPosition.x()) );
	glNormalPointer( GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iNormal.x()) );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iTexCoord.x()));

	glVertexAttribPointer(this->iVertexTangentAttribObject->getIndex(), 3, GL_FLOAT, 0, sizeof(Vertex), 
		&(this->iVertices[0].iTangent.x()));
	glVertexAttribPointer(this->iVertexTBNNormalAttribObject->getIndex(), 3, GL_FLOAT, 0, sizeof(Vertex), 
		&(this->iVertices[0].iTBNNormal.x()));
	glVertexAttribPointer(this->iVertexBinormalAttribObject->getIndex(), 3, GL_FLOAT, 0, sizeof(Vertex), 
		&(this->iVertices[0].iBinormal.x()));

	// the actual drawing
	glDrawElements(GL_TRIANGLES, this->iIndicesCount, GL_UNSIGNED_INT, &(this->iIndices.front()));

	glDisableVertexAttribArray( this->iVertexTangentAttribObject->getIndex() );
	glDisableVertexAttribArray( this->iVertexTBNNormalAttribObject->getIndex() );
	glDisableVertexAttribArray( this->iVertexBinormalAttribObject->getIndex() );
	glDisableClientState( GL_VERTEX_ARRAY );	
	glDisableClientState( GL_NORMAL_ARRAY );	
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

#else
	Vertex v;
	Vector3<float> vec, norm;
	glLineWidth( 1.0f );
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < this->iIndicesCount; i++)
		{
			v = this->iVertices[ this->iIndices[ i ] ];
			vec = v.iPosition;
			norm = v.iNormal;
			glVertex3f(vec.x(), vec.y(), vec.z());
			glNormal3f(norm.x(), norm.y(), norm.z());
		}
	glEnd();

	glDisable(GL_LIGHTING);
	glColor3f(1, 0, 1);
	glLineWidth( 2.0f );
	float len = 10;
	glBegin(GL_LINES);
		for (int i = 0; i < this->iVertices.size(); i++)
		{
			v = this->iVertices[ i ];
			vec = v.iPosition;
			norm = v.iNormal;
			glVertex3f(vec.x(), vec.y(), vec.z());
			glVertex3f(vec.x() + norm.x() * len, vec.y() + norm.y() * len, vec.z() + norm.z() * len);
		}
	glEnd();
#endif
}

/**
 */
void Mesh::createInverseTBNMatrices()
{
	TTriangleIterator iterator( this );
	GLuint indices[3];

	while ( iterator.getNextTriangle( indices ) )
	{
		Vertex& v0 = this->geVertex( indices[0] );
		Vertex& v1 = this->geVertex( indices[1] );
		Vertex& v2 = this->geVertex( indices[2] );

		v0.iTBNNormal.set( 0, 0 ,0 );
		v1.iTBNNormal.set( 0, 0 ,0 );
		v2.iTBNNormal.set( 0, 0 ,0 );

		createInverseTBNMatrix( v0, v1, v2 );
		createInverseTBNMatrix( v1, v2, v0 );
		createInverseTBNMatrix( v2, v0, v1 );
	}

	vector<Vertex>::iterator vertexIterator = iVertices.begin();
	for (;vertexIterator != iVertices.end(); vertexIterator++)
	{
		(*vertexIterator).iTBNNormal.Normalize();
		(*vertexIterator).iTangent.Normalize();
		(*vertexIterator).iBinormal.Normalize();
	}
}

/************************************************************************/
/* CONVENIENCE CLASS TTRIANGLEITERATOR                                  */
/************************************************************************/
TTriangleIterator::TTriangleIterator(Mesh* aMesh)
: iMesh( aMesh )
{
	this->iCurrentTriangle[0] = 0;
	this->iCurrentTriangle[1] = 1;
	this->iCurrentTriangle[2] = 2;
}

bool TTriangleIterator::getNextTriangle(GLuint* aIndices)
{
	if (this->iCurrentTriangle[2] < static_cast<GLuint>( this->iMesh->getIndicesCount() ))
	{
		this->iMesh->getTriangleIndices(this->iCurrentTriangle, aIndices);

		this->iCurrentTriangle[0] += 3;
		this->iCurrentTriangle[1] += 3;
		this->iCurrentTriangle[2] += 3;

		return true;
	}

	return false;
}

/**
 * @description NOTE: the inverse TBN vectors are added and not substituted. 
 *              The TBN vectors are NOT normalized.
 * @param aVertices
 * @param aTextureCoordinates
 * @param aInverseNormal
 * @param aInverseBinormal
 * @param aInverseTangent
 */
void Mesh::createInverseTBNMatrix(Vertex& aVertex0, 
								   const Vertex& aVertex1, 
								   const Vertex& aVertex2)
{
	Vector3<float> v0v1 = aVertex1.iPosition - aVertex0.iPosition;
	Vector3<float> v0v2 = aVertex2.iPosition - aVertex0.iPosition;

	float c0c1_T = aVertex1.iTexCoord.x() - aVertex0.iTexCoord.x();
	float c0c1_B = aVertex1.iTexCoord.y() - aVertex0.iTexCoord.y();

	float c0c2_T = aVertex2.iTexCoord.x() - aVertex0.iTexCoord.x();
	float c0c2_B = aVertex2.iTexCoord.y() - aVertex0.iTexCoord.y();

	float determinant = c0c1_T*c0c2_B - c0c2_T*c0c1_B;

	// if its very close to zero, create an orthogonal system by hand
	if ( biasToZero(determinant) )
	{
		aVertex0.iTangent.add(	1.0f, 0.0f, 0.0f);
		aVertex0.iBinormal.add(	0.0f, 1.0f, 0.0f);
		aVertex0.iTBNNormal.add(0.0f, 0.0f, 1.0f);
	}
	else
	{
		float invDeterminant1 = 1.0f / determinant;

		Vector3<float> T = Vector3<float>( 
			(c0c2_B  * v0v1.x() - c0c1_B * v0v2.x()) * invDeterminant1,
			(c0c2_B  * v0v1.y() - c0c1_B * v0v2.y()) * invDeterminant1,
			(c0c2_B  * v0v1.z() - c0c1_B * v0v2.z()) * invDeterminant1 );

		Vector3<float> B = Vector3<float>( 
			(c0c1_T * v0v2.x() - c0c2_T * v0v1.x()) * invDeterminant1,
			(c0c1_T * v0v2.y() - c0c2_T * v0v1.y()) * invDeterminant1,
			(c0c1_T * v0v2.z() - c0c2_T * v0v1.z()) * invDeterminant1 );

		Vector3<float> N = T^B;

		float invDeterminant2 = 1.0f / ( 
			(T.x() * B.y() * N.z() - T.z() * B.y() * N.x()) +
			(B.x() * N.y() * T.z() - B.z() * N.y() * T.x()) +
			(N.x() * T.y() * B.z() - N.z() * T.y() * B.x()) );

		// In these three cases the inverse TBN is stored
		aVertex0.iTangent.add(  
			(B^N).x() * invDeterminant2,
			((-N)^T).x() * invDeterminant2,
			(T^B).x() * invDeterminant2 );

		aVertex0.iBinormal.add( 
			((-B)^N).y() * invDeterminant2,
			(N^T).y() * invDeterminant2,
			((-T)^B).y() * invDeterminant2 );

		aVertex0.iTBNNormal.add( 
			(B^N).z() * invDeterminant2,
			((-N)^T).z() * invDeterminant2,
			(T^B).z() * invDeterminant2 );
	}
}