uniform sampler2D originalTex;
uniform sampler2D luminanceTex;
uniform float logAverage;
uniform float exposure;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = texture2D(originalTex, gl_TexCoord[0].st);
	float Lw = texture2D(luminanceTex, gl_TexCoord[0].st).x;

	float Lm = exposure * Lw / logAverage;
	float Ld = Lm / (Lm + 1.0);
	
	color = decodeRGBE(color);

	color.rgb = vec3(Ld, Ld, Ld) * color.rgb / Lw;

	gl_FragColor = color;
}