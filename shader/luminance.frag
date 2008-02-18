uniform sampler2D inputTexture;
//uniform vec2 scaleBias;

float computeLuminance(vec4 color);
vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(float luminance);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(inputTexture, gl_TexCoord[0].st);

	color = decodeRGBE(color);

	//if(color.r > 1.0 || color.g > 1.0 || color.b > 1.0)
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

	float luminance = computeLuminance(color);

	//gl_FragColor = vec4(luminance, luminance, luminance, 1.0);

	vec4 L = encodeRGBE(luminance);

	//if(L.r < 0.99)
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

	gl_FragColor = L;
}

float computeLuminance(vec4 color)
{
	float maxRGB = max(max(color.r, color.g), color.b);
	float minRGB = min(min(color.r, color.g), color.b);

	return 0.5 * (maxRGB + minRGB);
}

vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 255.0 - 128.0);
	return color * exponent;
}

/*
 * @param luminance it's the same for all channels,
 * so, it's considered as the max of all the channels
 * and, therefore, we only encode based on it
 */
vec4 encodeRGBE(float luminance)
{
	// NOTE: skipping ceil and the multiplication per 255.0 
	// because we have floats instead of integer
	float exponent = log2(luminance + 10e-8);
	float encL = luminance / exp2( exponent );
	float encE = (exponent + 128.0) / 255.0;

	return vec4(encL, encL, encL, encE);
}

vec4 encodeRGBE(vec4 color)
{
	float maxChannel = max(max(color.r, color.g), color.b);
	float exponent = ceil( log2(maxChannel * 255.0) );
	color.rgb /= exp2( exponent );
	color.a = (exponent + 128.0) / 255.0;

	return color;
}

//vec4 computeLuminance(vec4 color)
//{
//	float maxRGB = max(max(color.r, color.g), color.b);
//	float minRGB = min(min(color.r, color.g), color.b);
//
//	float L = 0.5 * (maxRGB + minRGB);
//
//	return vec4(L, L, L, 1.0);
//}