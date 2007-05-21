uniform samplerCube reflectionCubeMap;
uniform samplerCube diffuseCubeMap;

varying vec3 N;
varying vec3 V;
varying float reflectance;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

const float cutoff = 0.4;

void main(void)
{
	vec3 normR = normalize( reflect(V, N) );
	vec3 normN = normalize(N);

	vec4 reflectionColor = decodeRGBE( textureCube(reflectionCubeMap, normR) );
	reflectionColor *= reflectance;

	vec4 diffuseColor = decodeRGBE( textureCube(diffuseCubeMap, normN) );

	vec4 finalColor = reflectionColor + diffuseColor * cutoff;

	gl_FragColor = encodeRGBE( finalColor );
}