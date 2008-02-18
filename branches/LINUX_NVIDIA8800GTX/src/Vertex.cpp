#include "Vertex.h"

Vertex::Vertex(void)
{
}

Vertex::Vertex(const Vector3<float>& aNormal, const Vector3<float>& aPosition, 
				 const Vector2<float>& aTexCoord)
: iNormal( aNormal )
, iPosition( aPosition )
, iTexCoord( aTexCoord )
{
}

Vertex::~Vertex(void)
{
}
