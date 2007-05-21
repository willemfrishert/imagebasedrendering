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
	//vec4 color2 = decodeRGBE(texture2D( blurTexture0,    texCoord ));
	//vec4 color3 = decodeRGBE(texture2D( blurTexture1,    texCoord ));
	//vec4 color4 = decodeRGBE(texture2D( blurTexture2,    texCoord ));
	//vec4 color5 = decodeRGBE(texture2D( blurTexture3,    texCoord ));

	vec4 color2 = (texture2D( blurTexture0,    texCoord ) * 256.0);
	vec4 color3 = (texture2D( blurTexture1,    texCoord ) * 256.0);
	vec4 color4 = (texture2D( blurTexture2,    texCoord ) * 256.0);
	vec4 color5 = (texture2D( blurTexture3,    texCoord ) * 256.0);
	fragColor.rgb += color1.rgb;
	fragColor.rgb += color2.rgb * 0.8;
	fragColor.rgb += color3.rgb * 0.2;
	fragColor.rgb += color4.rgb * 0.1;
	fragColor.rgb += color5.rgb * 0.1;

	//float c = 1.2;
	//if ( (color2.r > c) ||(color2.g > c) || (color2.b > c) )
	//{
	//	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//}

	//fragColor = color2 * 0.1;

	gl_FragColor = encodeRGBE(fragColor);
	//gl_FragColor = vec4(fragColor.rgb, 1.0);
}