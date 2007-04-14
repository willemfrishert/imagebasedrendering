uniform sampler2D irradianceMap;
varying vec3 refraction;
varying vec3 reflection;

void main(void)
{
	vec2 texCoord;
	//vec3 normR = normalize(R);

	// Map the vector's cartesian coordinates to UV coordinates
	texCoord.s = 0.5 * (1.0 + 0.3183098862 * atan(refraction.x, -refraction.z));
	texCoord.t = 0.3183098862 * acos(refraction.y);

	vec4 color = texture2D(irradianceMap, texCoord);

	texCoord.s = 0.5 * (1.0 + 0.3183098862 * atan(reflection.x, -reflection.z));
	texCoord.t = 0.3183098862 * acos(reflection.y);

	color += texture2D(irradianceMap, texCoord);
	
	gl_FragColor = color;
}