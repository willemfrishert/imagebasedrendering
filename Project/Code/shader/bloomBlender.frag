uniform sampler2D originalTexture;
uniform sampler2D blurTexture0;
uniform sampler2D blurTexture1;
uniform sampler2D blurTexture2;
uniform sampler2D blurTexture3;

void main(void)
{
	vec2 texCoord = gl_TexCoord[0].xy;

	vec3 fragColor;
	fragColor += texture2D( originalTexture, texCoord ).rgb;
	fragColor += texture2D( blurTexture0,    texCoord ).rgb;
	fragColor += texture2D( blurTexture1,    texCoord ).rgb;
	fragColor += texture2D( blurTexture2,    texCoord ).rgb;
	fragColor += texture2D( blurTexture3,    texCoord ).rgb;

	gl_FragColor = vec4(fragColor, 1.0);
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); //red
}