uniform sampler2DRect inputTexture;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	// NOTE: textureRect not supported on ATI
    float left		= texture2DRect(inputTexture, topLeft).x;
    float right		= texture2DRect(inputTexture, topRight).x;
    float top		= texture2DRect(inputTexture, bottomLeft).x;
    float bottom	= texture2DRect(inputTexture, bottomRight).x;
	
	float maxValue = max(max(left, right), max(top, bottom));
	gl_FragColor = vec4(maxValue, maxValue, maxValue, 1);
	//gl_FragColor = vec4(left.x, left.y, left.z, left.w);
}