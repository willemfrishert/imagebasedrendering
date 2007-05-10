uniform samplerCube reflectionCubeMap;
uniform samplerCube diffuseCubeMap;

//varying vec3 R;
varying vec3 N;
varying vec3 V;
//varying float Ks;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

//float computeLuminance(vec4 color);

void main(void)
{
	vec2 texCoord;

	//vec3 normR = normalize(R);
	vec3 normR = normalize( reflect(V, N) );
	vec3 normN = normalize(N);

	//float Ks = pow(dot(-V, N), 50.0);

	// Map the vector's cartesian coordinates to UV coordinates

	//vec4 reflectionColor = decodeRGBE( texture2D(irradianceMap, texCoord) );
  
	//vec4 matColor = vec4(0.8, 0.8, 0.8, 1.0);// * vec4(Kd, Kd, Kd, 1.0);
	//vec4 finalColor = mix(matColor, reflectionColor, Ks);
	vec4 reflectionColor = decodeRGBE( textureCube(reflectionCubeMap, normR) );

	float maxChannel = max( reflectionColor.r, max(reflectionColor.g, reflectionColor.b) );

	if (maxChannel < 2.0)
	{
		const float threshold = 0.25;
		reflectionColor *= threshold;
	}

	vec4 diffuseColor = decodeRGBE( textureCube(diffuseCubeMap, normN) );

	vec4 finalColor = reflectionColor + diffuseColor;
	finalColor = reflectionColor * pow( dot(normN, normalize(-V) ), 5.0);
	finalColor += diffuseColor;

//	finalColor = mix( reflectionColor,diffuseColor, vec4(0.75) );
//	finalColor = max( reflectionColor,diffuseColor );

	//gl_FragColor = textureCube(reflectionCubeMap, normN);
	//gl_FragColor = texture2D(irradianceMap, texCoord);

	//float maxColor = max(reflectionColor.r, max(reflectionColor.g ,reflectionColor.b));
	//vec4 color = vec4(maxColor, maxColor,maxColor, reflectionColor.a);
	//finalColor = color;

	gl_FragColor = encodeRGBE( finalColor );
	//gl_FragColor = vec4(normN.x, normN.y, normN.z, 1.0);
	//gl_FragColor = reflectionColor;
	//gl_FragColor = encodeRGBE( reflectionColor );
}