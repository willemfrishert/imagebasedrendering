uniform samplerCube reflectionCubeMap;
//varying vec3 R;
varying vec3 N;
varying vec3 V;
//varying float Ks;

//vec4 decodeRGBE(vec4 color);
//vec4 encodeRGBE(vec4 color);

//float computeLuminance(vec4 color);

void main(void)
{
	//vec2 texCoord;

	//vec3 normR = normalize(R);
	vec3 normR = normalize( reflect(V, N) );

	//float Ks = pow(dot(-V, N), 50.0);

	//// Map the vector's cartesian coordinates to UV coordinates
	//texCoord.s = 0.5 * (1.0 + 0.3183098862 * atan(normR.x, -normR.z));
	//texCoord.t = 0.3183098862 * acos(normR.y);

	//vec4 reflectionColor = decodeRGBE( texture2D(irradianceMap, texCoord) );
  
	//vec4 matColor = vec4(0.8, 0.8, 0.8, 1.0);// * vec4(Kd, Kd, Kd, 1.0);
	//vec4 finalColor = mix(matColor, reflectionColor, Ks);
	////vec4 finalColor = reflectionColor + matColor * Kd;

	gl_FragColor = textureCube(reflectionCubeMap, normR);
	//gl_FragColor = texture2D(irradianceMap, texCoord);
}

//float computeLuminance(vec4 color)
//{
//	return dot(vec3(0.27, 0.67, 0.06), color.rgb);
//}