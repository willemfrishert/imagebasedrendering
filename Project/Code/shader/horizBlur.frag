uniform sampler2D originalTexture;
uniform int mipmapSize;
uniform float mipmapLevel;

uniform float blurWeight1;
uniform float blurWeight2;
uniform float blurWeight3;

//varying vec2 fragPosition;

//vec4 decodeRGBE(vec4 color);
//vec4 encodeRGBE(vec4 color);

void main(void)
{
	//// initial value
	vec4 fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	float delta = 1.0/float(mipmapSize);

	vec2 texCoord = gl_TexCoord[0].st;

	vec4 color1 = texture2D(originalTexture, texCoord.st + vec2( -2.0, 0.0 )*delta );
	vec4 color2 = texture2D(originalTexture, texCoord.st + vec2( -1.0, 0.0 )*delta );
	vec4 color3 = texture2D(originalTexture, texCoord.st                           );
	vec4 color4 = texture2D(originalTexture, texCoord.st + vec2(  1.0, 0.0 )*delta );
	vec4 color5 = texture2D(originalTexture, texCoord.st + vec2(  2.0, 0.0 )*delta );

	//color1 = decodeRGBE(color1);
	//color2 = decodeRGBE(color2);
	//color3 = decodeRGBE(color3);
	//color4 = decodeRGBE(color4);
	//color5 = decodeRGBE(color5);

	fragColor.rgb += blurWeight3 * color1.rgb;
	fragColor.rgb += blurWeight2 * color2.rgb;
	fragColor.rgb += blurWeight1 * color3.rgb;
	fragColor.rgb += blurWeight2 * color4.rgb;
	fragColor.rgb += blurWeight3 * color5.rgb;

	//gl_FragColor = encodeRGBE(fragColor);
	gl_FragColor = vec4(fragColor.rgb, 1.0);

	//DEBUG!!!

	//if (fragPosition.x>64.0)
	//{
	//	if (fragPosition.y>64.0)
	//	{
	//		gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); //blue
	//	}
	//	else
	//	{
	//		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); //red
	//	}
	//}
	//else
	//{	
	//	if (fragPosition.y>64.0)
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0); //cyan
	//	}
	//	else
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); //green
	//	}
	//}
 }