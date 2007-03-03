uniform sampler2D originalTexture;
uniform int mipmapSize;
//uniform float level;

void main(void)
{
	// initial value
	vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
	float delta = 1.0/float(mipmapSize);

	// the blur displacement

	// gl_FragCoord.x = [viewport.x/2, viewport.x]
	// gl_FragCoord.y = [viewport.y/2, viewport.y]
	// Need to rescale this to [0..1] in order to index the 2D texture
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 texCoord = gl_TexCoord[0].xy;

	fragColor.rgb += 0.0002 * texture2D(originalTexture, (fragCoord + vec2( -2.0, 0.0 )) * delta ).rgb;
	fragColor.rgb += 0.0837 * texture2D(originalTexture, (fragCoord + vec2( -1.0, 0.0 )) * delta ).rgb;
	fragColor.rgb += 0.6187 * texture2D(originalTexture, (fragCoord                    ) * delta ).rgb;
	fragColor.rgb += 0.0837 * texture2D(originalTexture, (fragCoord + vec2(  1.0, 0.0 )) * delta ).rgb;
	fragColor.rgb += 0.0002 * texture2D(originalTexture, (fragCoord + vec2(  2.0, 0.0 )) * delta ).rgb;


	//fragColor += texture2D(originalTexture, texCoord );

	//fragColor = texture2D(originalTexture, texCoord );
	gl_FragColor = vec4(fragColor.rgb, 1.0);

	//gl_FragColor = vec4(texture2D(originalTexture, gl_TexCoord[0].xy).rgb, 1.0);
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