uniform sampler2D originalTexture;
uniform sampler2D blurTexture0;
uniform sampler2D blurTexture1;
uniform sampler2D blurTexture2;
uniform sampler2D blurTexture3;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	vec2 texCoord = gl_TexCoord[0].st;

	vec4 fragColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 color1 = decodeRGBE(texture2D( originalTexture, texCoord ));
	vec4 color2 = (texture2D( blurTexture0,    texCoord ) * 256.0);
	vec4 color3 = (texture2D( blurTexture1,    texCoord ) * 256.0);
	vec4 color4 = (texture2D( blurTexture2,    texCoord ) * 256.0);
	vec4 color5 = (texture2D( blurTexture3,    texCoord ) * 256.0);
	
	fragColor.rgb += color1.rgb;
	fragColor.rgb += color2.rgb * 0.2;
	fragColor.rgb += color3.rgb * 0.2;
	fragColor.rgb += color4.rgb * 0.2;
	fragColor.rgb += color5.rgb * 0.2;

	// Vignette Effect
	texCoord -= 0.5;
	float vignette = 1.0 - dot(texCoord, texCoord);

	vignette = vignette * vignette * vignette * vignette;

	fragColor.rgb *= vignette;

	gl_FragColor = encodeRGBE(fragColor);
}