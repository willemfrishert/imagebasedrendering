uniform samplerCube diffuseCubeMap;

varying vec3 N;
varying vec3 V;
varying float ratio;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

//float computeLuminance(vec4 color);

void main(void)
{
	vec2 texCoord;
	vec3 normN = normalize(N);


	vec4 diffuseColor = decodeRGBE( textureCube(diffuseCubeMap, normN) );

	gl_FragColor = encodeRGBE( diffuseColor );
	//gl_FragColor = vec4(normN.x, normN.y, normN.z, 1.0);
	//gl_FragColor = reflectionColor;
	//gl_FragColor = encodeRGBE( reflectionColor );
}