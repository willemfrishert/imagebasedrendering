uniform samplerCube reflectionCubeMap;
varying vec3 refraction;
varying vec3 reflection;
varying float reflectance;
varying float transmittance;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec4 refractColor = decodeRGBE( textureCube(reflectionCubeMap, refraction) );
	vec4 reflectColor = decodeRGBE( textureCube(reflectionCubeMap, reflection) );
	
	float fresnelT = transmittance * 0.5;	
	float fresnelR = reflectance * 0.5;	

	gl_FragColor = encodeRGBE( reflectColor + refractColor);

	
//	gl_FragColor = color;
}