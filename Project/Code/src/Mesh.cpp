#include "ShaderAttributeObject.h"
#include "Mesh.h"

Mesh::Mesh()
: iIndicesCount( 0 )
, iTextured( false )
{
}

Mesh::~Mesh(void)
{
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
	
	if (iTextured) glEnableClientState( GL_TEXTURE_COORD_ARRAY );  
	
	// Passing the information to openGL using pointers to the actual arrays
	glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iPosition.x()) );
	glNormalPointer( GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iNormal.x()) );
	if (iTextured) glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), &(this->iVertices[0].iTexCoord.x()));

	// the actual drawing
	glDrawElements(GL_TRIANGLES, this->iIndicesCount, GL_UNSIGNED_INT, &(this->iIndices.front()));

	glDisableClientState( GL_VERTEX_ARRAY );	
	glDisableClientState( GL_NORMAL_ARRAY );	
	if (iTextured) glDisableClientState( GL_TEXTURE_COORD_ARRAY );

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