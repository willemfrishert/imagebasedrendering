uniform samplerCube cubeMap;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = textureCube(cubeMap, gl_TexCoord[0].xyz);

	//if(color.r > 1.0 || color.g > 1.0 || color.b > 1.0)
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
	//gl_FragColor = vec4(color.a, color.a, color.a, 1.0);

	//gl_FragColor = vec4(color.r, color.g, color.b, 1.0);

	gl_FragColor = decodeRGBE(color);
}