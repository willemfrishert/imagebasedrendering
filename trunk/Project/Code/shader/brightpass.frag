uniform sampler2D originalTexture;
uniform float logAvgLum;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);
float computeLuminance(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTexture, gl_TexCoord[0].st);

	vec4 colorDecoded = decodeRGBE(color);

	float luminance = computeLuminance(colorDecoded);

	float logLum = log(luminance + 1e-8);

	if (logAvgLum < logLum)
		//gl_FragColor =  vec4(colorDecoded.rgb, 1.0);
		gl_FragColor = color;
	else
		gl_FragColor = vec4(0.0,0.0,0.0,0.5);

	//gl_FragColor = vec4(luminance,luminance,luminance,1.0);
}

float computeLuminance(vec4 color)
{
	float maxRGB = max(max(color.r, color.g), color.b);
	float minRGB = min(min(color.r, color.g), color.b);

	return 0.5 * (maxRGB + minRGB);
}