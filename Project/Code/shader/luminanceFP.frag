uniform sampler2D inputTexture;
//uniform vec2 scaleBias;

float computeLuminance(vec4 color);
vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(inputTexture, gl_TexCoord[0].st);

	color = decodeRGBE(color);

	//if(color.r > 1.0 || color.g > 1.0 || color.b > 1.0)
	//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

	float luminance = computeLuminance(color);

	gl_FragColor = vec4(luminance, luminance, luminance, 1.0);
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