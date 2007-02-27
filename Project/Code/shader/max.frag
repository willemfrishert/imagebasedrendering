uniform sampler2DRect inputTexture;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	// NOTE: textureRect not supported on ATI
	//vec2 topleft	 = ((gl_FragCoord.st-0.5)*2.0)+0.5;
 //   float left		= texture2DRect(inputTexture, topleft).x;
 //   float right		= texture2DRect(inputTexture, topleft + vec2(1.0, 0.0)).x;
 //   float top		= texture2DRect(inputTexture, topleft + vec2(1.0, 1.0)).x;
 //   float bottom	= texture2DRect(inputTexture, topleft + vec2(0.0, 1.0)).x;

	//vec4 left		= texture2DRect(inputTexture, topLeft);

    float left		= texture2DRect(inputTexture, topLeft).x;
    float right		= texture2DRect(inputTexture, topRight).x;
    float top		= texture2DRect(inputTexture, bottomLeft).x;
    float bottom	= texture2DRect(inputTexture, bottomRight).x;
	
	float maxValue = max(max(left, right), max(top, bottom));
	gl_FragColor = vec4(maxValue, maxValue, maxValue, 1);
	//gl_FragColor = vec4(left.x, left.y, left.z, left.w);
}