uniform sampler2DRect inputTexture;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;


void main(void)
{
	// The values should be in the format (L, L, L, E) using RGBE encoding
	// Decode and then get Luminance (.x)
    float left		= texture2DRect(inputTexture, topLeft).x;
    float right		= texture2DRect(inputTexture, topRight).x;
    float top		= texture2DRect(inputTexture, bottomLeft).x;
    float bottom	= texture2DRect(inputTexture, bottomRight).x;
	
	// sum them all
	float sum = log(left + 10e-8) + log(right + 10e-8) + log(bottom + 10e-8) + log(top + 10e-8);
	//float sum = left + right + bottom + top;

	gl_FragColor = vec4(sum, sum, sum, 1.0);
}