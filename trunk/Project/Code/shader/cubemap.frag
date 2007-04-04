uniform samplerCube cubeMap;

vec4 decodeRGBE(vec4 color)
{
	// scale: max - min, bias: min
	//float scale = 7.0 / 255.0;
	//float bias	= 122.0 / 255.0;
	//vec2 scaleBias = vec2(scale, -scale/bias);
	//
	//vec3 decodedColor = color.rgb * exp2(color.a * scaleBias.x + scaleBias.y);

	//return vec4(decodedColor, 1.0);

	float exponent = exp2(color.a * 256.0 - 128.0);
	return color * exponent;
}

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