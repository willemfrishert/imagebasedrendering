uniform sampler2D originalTex;
uniform sampler2D luminanceTex;
uniform float logAverage;
uniform float exposure;
uniform float sensitivity;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTex, gl_TexCoord[0].st);
	float luminance = texture2D(luminanceTex, gl_TexCoord[0].st).x;

	/**** ***/
	float Lw = luminance / logAverage;
	float dummie = logAverage * exposure;
	/**** ***/

	float lumPow = pow(luminance, sensitivity);
	float sigmaN = pow(exposure, sensitivity);
	float Ld = lumPow / (lumPow + sigmaN);
	
	color = decodeRGBE(color);
	
	//float sigmaN = pow(exposure, sensitivity);
	//float rN = pow(color.r, sensitivity);
	//float gN = pow(color.g, sensitivity);
	//float bN = pow(color.b, sensitivity);
	//
	//color.r = rN / (rN + sigmaN);
	//color.g = gN / (gN + sigmaN);
	//color.b = bN / (bN + sigmaN);

	// power the quocient to a saturation constant s
	vec3 saturation = pow(color.rgb/luminance, vec3(0.8));
	color.rgb = vec3(Ld, Ld, Ld) * saturation;

	//color.r = rN;
	//color.g = gN;
	//color.b = bN;

	gl_FragColor = color;
}

vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 255.0 - 128.0);
	return color * exponent;
}