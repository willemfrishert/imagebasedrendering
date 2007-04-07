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

Vertex::Vertex(const Vector3<float>& aNormal, const Vector3<float>& aPosition, const Vector2<float>& aTexCoord, 
				 const Vector3<float>& aBinormal, const Vector3<float>& aTangent)
: iNormal( aNormal )
, iPosition( aPosition )
, iBinormal( aBinormal )
, iTexCoord( aTexCoord )
, iTangent( aTangent )
{
}

Vertex::~Vertex(void)
{
}
