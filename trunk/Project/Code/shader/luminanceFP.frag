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
}

float computeLuminance(vec4 color)
{
	return dot(vec3(0.27, 0.67, 0.06), color.rgb);
}