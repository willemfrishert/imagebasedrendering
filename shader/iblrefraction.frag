uniform samplerCube reflectionCubeMap;
varying vec3 refraction;
varying vec3 reflection;

varying float reflectance;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec3 refractColor = decodeRGBE( textureCube(reflectionCubeMap, refraction) ).rgb;
	vec3 reflectColor = decodeRGBE( textureCube(reflectionCubeMap, reflection) ).rgb;
	
	// reflectance is the reflectance coefficient
	vec3 color = mix(refractColor, reflectColor, reflectance);

	color = max(color, vec3(1e-4));

	gl_FragColor = encodeRGBE( vec4(color, 1.0) );
}
