uniform sampler2D originalTex;
uniform sampler2D luminanceTex;
uniform float logAverage;
uniform float exposure;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTex, gl_TexCoord[0].st);
	float Lw = texture2D(luminanceTex, gl_TexCoord[0].st).x;

	float Lm = exposure * Lw;
	float Ld = Lm / (Lm + 1.0);
	
	color = decodeRGBE(color);

	//color.rgb = color.rgb * Ld / Lw;

	// WORKING
	color.rgb *= exposure;
	color.rgb = color.rgb / (color.rgb + vec3(1.0));

	//color.rgb = pow(color.rgb, vec3(0.8));

	logAverage * 1.0;

	gl_FragColor = color;
	//gl_FragColor = vec4(Ld, Ld, Ld, 1.0);
}