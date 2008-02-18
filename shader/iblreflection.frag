uniform samplerCube reflectionCubeMap;

varying vec3 N;
varying vec3 V;

void main(void)
{
	vec3 normR	= normalize( reflect(V, N) );
	gl_FragColor = textureCube(reflectionCubeMap, normR);
}

