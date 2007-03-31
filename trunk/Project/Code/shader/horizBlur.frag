uniform sampler2D originalTexture;
uniform int mipmapSize;
uniform float mipmapLevel;

void main(void)
{
	//// initial value
	vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
	float delta = 1.0/float(mipmapSize);

	//// the blur displacement

	//// gl_FragCoord.x = [viewport.x/2, viewport.x]
	//// gl_FragCoord.y = [viewport.y/2, viewport.y]
	//// Need to rescale this to [0..1] in order to index the 2D texture
	////vec2 fragCoord = (gl_FragCoord.xy-(textureSize/2.0))*delta;
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 texCoord = gl_TexCoord[0].xy;

	fragColor.rgb += 0.0002 * texture2D(originalTexture, (fragCoord + vec2( -2.0, 0.0 )) * delta, mipmapLevel ).rgb;
	fragColor.rgb += 0.0837 * texture2D(originalTexture, (fragCoord + vec2( -1.0, 0.0 )) * delta, mipmapLevel ).rgb;
	fragColor.rgb += 0.6187 * texture2D(originalTexture, (fragCoord                    ) * delta, mipmapLevel ).rgb;
	fragColor.rgb += 0.0837 * texture2D(originalTexture, (fragCoord + vec2(  1.0, 0.0 )) * delta, mipmapLevel ).rgb;
	fragColor.rgb += 0.0002 * texture2D(originalTexture, (fragCoord + vec2(  2.0, 0.0 )) * delta, mipmapLevel ).rgb;

	////fragColor += texture2D(originalTexture, texCoord );

	////fragColor = 0.1 * texture2D(originalTexture, texCoord, 1.0 );
	gl_FragColor = vec4(fragColor.rgb, 1.0);

	////gl_FragColor = vec4(texture2D( originalTexture,  fragCoord ).rgb, 1.0);


	//DEBUG!!!

	//if (fragCoord.x>16.0)
	//{
	//	if (fragCoord.y>16.0)
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
	//	if (fragCoord.y>16.0)
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0); //cyan
	//	}
	//	else
	//	{
	//		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); //green
	//	}
	//}
	//
	//gl_FragColor = vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 0.0);
	//gl_FragColor = vec4(texture2D(originalTexture, texCoord, 4.0 ).rgb, 1.0);

	//gl_FragColor = vec4( 0.5, 0.0, 0.5, 1.0);
 }