uniform sampler2D originalTex;
uniform sampler2D luminanceTex;
uniform float logAverage;
uniform float exposure;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTex, gl_TexCoord[0].st);
	float luminance = texture2D(luminanceTex, gl_TexCoord[0].st).x;

	float Lw = luminance / logAverage;
	
	float dummie = logAverage * exposure;
	//float Lw = luminance;
	
	/*float Ld = Lw / (Lw + 1.0);*/

	color = decodeRGBE(color);

	/*color *= (Ld / Lw);*/
	
	float n = 1.5;
	float sigmaN = pow(exposure, n);
	float rN = pow(color.r, n);
	float gN = pow(color.g, n);
	float bN = pow(color.b, n);
	
	color.r = rN / (rN + sigmaN);
	color.g = gN / (gN + sigmaN);
	color.b = bN / (bN + sigmaN);

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