uniform sampler2D originalTexture;
uniform int mipmapSize;
//uniform float mipmapLevel;

uniform float blurWeight1;
uniform float blurWeight2;
uniform float blurWeight3;

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(vec4 color);

void main(void)
{
	// initial value
	vec4 fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	float delta = 1.0/float(mipmapSize);

	// the blur displacement

	// gl_FragCoord.x = [viewport.x/2, viewport.x]
	// gl_FragCoord.y = [viewport.y/2, viewport.y]
	// Need to rescale this to [0..1] in order to index the 2D texture
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 texCoord = gl_TexCoord[0].xy;

	//fragColor.rgb += 0.01330373 * texture2D(originalTexture, (fragCoord + vec2( 0.0, -2.0 )) * delta ).rgb;
	//fragColor.rgb += 0.11098164 * texture2D(originalTexture, (fragCoord + vec2( 0.0, -1.0 )) * delta ).rgb;
	//fragColor.rgb += 0.22508352 * texture2D(originalTexture, (fragCoord                    ) * delta ).rgb;
	//fragColor.rgb += 0.11098164 * texture2D(originalTexture, (fragCoord + vec2( 0.0,  1.0 )) * delta ).rgb;
	//fragColor.rgb += 0.01330373 * texture2D(originalTexture, (fragCoord + vec2( 0.0,  2.0 )) * delta ).rgb;
	
	vec4 color1 = texture2D(originalTexture, (fragCoord + vec2( 0.0, -2.0 )) * delta );
	vec4 color2 = texture2D(originalTexture, (fragCoord + vec2( 0.0, -1.0 )) * delta );
	vec4 color3 = texture2D(originalTexture, (fragCoord                    ) * delta );
	vec4 color4 = texture2D(originalTexture, (fragCoord + vec2( 0.0,  1.0 )) * delta );
	vec4 color5 = texture2D(originalTexture, (fragCoord + vec2( 0.0,  2.0 )) * delta );
	
	color1 = decodeRGBE(color1);
	color2 = decodeRGBE(color2);
	color3 = decodeRGBE(color3);
	color4 = decodeRGBE(color4);
	color5 = decodeRGBE(color5);

	fragColor.rgb += blurWeight3 * color1.rgb;
	fragColor.rgb += blurWeight2 * color2.rgb;
	fragColor.rgb += blurWeight1 * color3.rgb;
	fragColor.rgb += blurWeight2 * color4.rgb;
	fragColor.rgb += blurWeight3 * color5.rgb;

	//fragColor.rgb += blurWeight3 * texture2D(originalTexture, (fragCoord + vec2( 0.0, -2.0 )) * delta ).rgb;
	//fragColor.rgb += blurWeight2 * texture2D(originalTexture, (fragCoord + vec2( 0.0, -1.0 )) * delta ).rgb;
	//fragColor.rgb += blurWeight1 * texture2D(originalTexture, (fragCoord                    ) * delta ).rgb;
	//fragColor.rgb += blurWeight2 * texture2D(originalTexture, (fragCoord + vec2( 0.0,  1.0 )) * delta ).rgb;
	//fragColor.rgb += blurWeight3 * texture2D(originalTexture, (fragCoord + vec2( 0.0,  2.0 )) * delta ).rgb;



	//float sum = blurWeight3 + blurWeight2 + blurWeight1 + 	blurWeight2 + 	blurWeight3;

	gl_FragColor = encodeRGBE(fragColor);
	//gl_FragColor = vec4(color3.rgb, 1.0);
//	gl_FragColor = vec4(1.0,1.0,1.0,1.0);
//	gl_FragColor = vec4(texture2D(originalTexture, gl_TexCoord[0].xy).rgb, 1.0);
//	gl_FragColor = texture2D(originalTexture, (fragCoord                    ) * delta );

	//DEBUG!!!

	//if (fragCoord.x>64.0)
	//{
	//	if (fragCoord.y>64.0)
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
	//	if (fragCoord.y>64.0)
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0); //cyan
	//	}
	//	else
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); //green
	//	}
	//}
}