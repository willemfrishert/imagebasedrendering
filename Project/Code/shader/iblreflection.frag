uniform sampler2D irradianceMap;
varying vec3 R;
varying vec3 N;
varying vec3 V;
varying float Kd;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec2 texCoord;

	vec3 normR = normalize(R);

	// Map the vector's cartesian coordinates to UV coordinates
	texCoord.s = 0.5 * (1.0 + 0.3183098862 * atan(normR.x, -normR.z));
	texCoord.t = 0.3183098862 * acos(normR.y);

	vec4 reflectionColor = decodeRGBE( texture2D(irradianceMap, texCoord) );

	//vec4 finalColor = mix(matColor, reflectionColor, Kd);
	vec4 finalColor = vec4(Kd, Kd, Kd, 1.0);

	//gl_FragColor = reflectionColor;
	gl_FragColor = encodeRGBE( reflectionColor );
}