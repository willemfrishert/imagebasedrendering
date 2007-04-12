uniform sampler2D originalTexture;
uniform sampler2D prevBrightpass;
uniform float logAvgLum;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);
float computeLuminance(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTexture, gl_TexCoord[0].st);
	vec4 colorPrevBP =  texture2D(prevBrightpass, gl_TexCoord[0].st);

	vec4 colorDecoded = decodeRGBE(color);
	colorPrevBP = decodeRGBE(colorPrevBP);

	float luminance = computeLuminance(colorDecoded);
	//float luminance = computeLuminance(color);
	float logLum = log(luminance + 1e-8);

	if (logAvgLum >= logLum)
	//if (0.99 < luminance)
		//gl_FragColor =  vec4(colorDecoded.rgb, 1.0);
		//gl_FragColor = colorDecoded;
	//else
		colorDecoded = vec4(0.0,0.0,0.0,1.0);

//	color = colorPrevBP * 0.9 + colorDecoded * 0.25 - vec4(0.0392156862745);
	color = mix(colorPrevBP, colorDecoded, 0.1);


	gl_FragColor = encodeRGBE(color);
	
	//gl_FragColor = encodeRGBE(colorDecoded);

	//gl_FragColor = vec4(luminance,luminance,luminance,1.0);
}

float computeLuminance(vec4 color)
{
	float maxRGB = max(max(color.r, color.g), color.b);
	float minRGB = min(min(color.r, color.g), color.b);

	return 0.5 * (maxRGB + minRGB);
}