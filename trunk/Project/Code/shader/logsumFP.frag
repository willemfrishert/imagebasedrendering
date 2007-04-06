uniform sampler2D inputTexture;
uniform float height;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	// NOTE: extract from THE SECOND CHANNEL: Y, where log(luminance)
	// should be stored
    float left		= texture2D(inputTexture, topLeft / height).y;
    float right		= texture2D(inputTexture, topRight / height).y;
    float top		= texture2D(inputTexture, bottomLeft / height).y;
    float bottom	= texture2D(inputTexture, bottomRight / height).y;
	
	// sum them all
	float sum = left + right + bottom + top;

	gl_FragColor = vec4(sum, sum, sum, right);
}