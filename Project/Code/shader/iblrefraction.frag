uniform samplerCube reflectionCubeMap;
varying vec3 refraction;
varying vec3 reflection;

varying float ratio;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec3 refractColor = decodeRGBE( textureCube(reflectionCubeMap, refraction) ).rgb;
	vec3 reflectColor = decodeRGBE( textureCube(reflectionCubeMap, reflection) ).rgb;
	
	vec3 color = mix(refractColor, reflectColor, ratio);

	color = max(color, vec3(1e-4));

	gl_FragColor = encodeRGBE( vec4(color, 1.0) );
}