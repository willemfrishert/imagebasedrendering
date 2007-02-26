uniform sampler2DRect inputTexture;

void main(void)
{
	// NOTE: textureRect not supported on ATI
	// get the fragment's texture color
	//float left		= texture2DRect(inputTexture, gl_TexCoord[0].xy).x;
	//float right		= texture2DRect(inputTexture, gl_TexCoord[1].xy).x;
	//float top		= texture2DRect(inputTexture, gl_TexCoord[2].xy).x;
	//float bottom	= texture2DRect(inputTexture, gl_TexCoord[3].xy).x;

	//float left	= texture2DRect(inputTexture, gl_FragCoord.st).x;
	//float right = texture2DRect(inputTexture, vec2(gl_FragCoord.s + 1.0, gl_FragCoord.t)).x;
	//float top	 = texture2DRect(inputTexture, vec2(gl_FragCoord.s + 1.0, gl_FragCoord.t + 1.0)).x;
	//float bottom = texture2DRect(inputTexture, vec2(gl_FragCoord.s, gl_FragCoord.t + 1.0)).x;

	//float left = texture2DRect(inputTexture, gl_TexCoord[0].xy).x;
 //   float right = texture2DRect(inputTexture, gl_TexCoord[0].xy+vec2(1.0,0.0)).x;
 //   float top = texture2DRect(inputTexture, gl_TexCoord[0].xy+vec2(0.0,1.0)).x;
 //   float bottom = texture2DRect(inputTexture, gl_TexCoord[0].xy+vec2(1.0,1.0)).x;

	vec2 topleft = ((gl_FragCoord.st-0.5)*2.0)+0.5;
    float left = texture2DRect(inputTexture, topleft).x;
    float right = texture2DRect(inputTexture, topleft+vec2(1.0,0.0)).x;
    float top = texture2DRect(inputTexture, topleft+vec2(1.0,1.0)).x;
    float bottom = texture2DRect(inputTexture, topleft+vec2(0.0,1.0)).x;
	
	float maxValue = max(max(left, right), max(top, bottom));
	//float sum = left + right + top + bottom;
	gl_FragColor = vec4(maxValue, maxValue, maxValue, 1);
	//gl_FragColor = vec4(gl_TexCoord[0].x, gl_TexCoord[0].y, gl_TexCoord[1].x, gl_TexCoord[1].y);
	//gl_FragColor = vec4(left, right, top, bottom);
}