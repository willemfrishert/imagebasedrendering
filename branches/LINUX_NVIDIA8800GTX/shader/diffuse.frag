uniform samplerCube diffuseCubeMap;

varying vec3 N;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

const float cutoff = 0.4;

void main(void)
{
	vec3 normN = normalize(N);
	vec4 diffuseColor = decodeRGBE( textureCube(diffuseCubeMap, normN) );

	gl_FragColor = encodeRGBE( diffuseColor * cutoff );
}