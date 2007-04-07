uniform sampler2D inputTexture;

float computeLuminance(vec4 color);
vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(inputTexture, gl_TexCoord[0].st);

	color = decodeRGBE(color);

	float luminance = computeLuminance(color);

	// Store luminance on the first channel and log(luminance) on the second
	gl_FragColor = vec4(luminance, log(luminance + 10e-8), luminance, luminance);
	
	//float delta = 1.0;
	//if(color.r > delta || color.g > delta || color.b > delta)
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

	//gl_FragColor = color;
	//gl_FragColor = vec4(luminance, luminance, luminance, 1.0);
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
	color.rgb *= exponent;
	color.a = 1.0;
	return color;
}