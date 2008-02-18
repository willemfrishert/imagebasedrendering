uniform sampler2D inputTexture;
uniform float height;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	// NOTE: textureRect not supported on ATI
    float left		= texture2D(inputTexture, topLeft / height).x;
    float right		= texture2D(inputTexture, topRight / height).x;
    float top		= texture2D(inputTexture, bottomLeft / height).x;
    float bottom	= texture2D(inputTexture, bottomRight / height).x;
	
	float maxValue = max(max(left, right), max(top, bottom));
	gl_FragColor = vec4(maxValue, maxValue, maxValue, maxValue);
}