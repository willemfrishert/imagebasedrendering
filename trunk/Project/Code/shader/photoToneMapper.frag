uniform sampler2D originalTex;
uniform float exposure;

vec4 decodeRGBE(vec4 color);

void main(void)
{
	vec4 color = decodeRGBE( texture2D(originalTex, gl_TexCoord[0].st) );

	// Reinhard Photographic Tone Mapper - not using luminance information
	color.rgb *= exposure;
	color.rgb = color.rgb / (color.rgb + vec3(1.0));

	gl_FragColor = color;
}