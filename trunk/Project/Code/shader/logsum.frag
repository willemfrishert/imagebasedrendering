uniform sampler2DRect inputTexture;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	vec4 left	= texture2DRect(inputTexture, topLeft);
    vec4 right	= texture2DRect(inputTexture, topRight);
    vec4 top	= texture2DRect(inputTexture, bottomLeft);
    vec4 bottom	= texture2DRect(inputTexture, bottomRight);

	// decode the pixels
	vec4 leftRGB	= left;
	vec4 rightRGB	= right;
	vec4 topRGB		= topRGB;
	vec4 bottomRGB	= bottomRGB;
	
	//// convert to luminance values
	//float maxChannel = max(max(leftRGB.r, leftRGB.g), leftRGB.b);
	//float minChannel = min(min(leftRGB.r, leftRGB.g), leftRGB.b);
	//float l1 = 0.5 * (maxChannel, minChannel);

	//maxChannel = max(max(rightRGB.r, rightRGB.g), rightRGB.b);
	//minChannel = min(min(rightRGB.r, rightRGB.g), rightRGB.b);
	//float l2 = 0.5 * (maxChannel, minChannel);

	//maxChannel = max(max(topRGB.r, topRGB.g), topRGB.b);
	//minChannel = min(min(topRGB.r, topRGB.g), topRGB.b);
	//float l3 = 0.5 * (maxChannel, minChannel);

	//maxChannel = max(max(bottomRGB.r, bottomRGB.g), bottomRGB.b);
	//minChannel = min(min(bottomRGB.r, bottomRGB.g), bottomRGB.b);
	//float l4 = 0.5 * (maxChannel, minChannel);

	// sum them all
	float sum = log(l1 + l2 + l3 + l4 + 10e-8);

	// encode

	gl_FragColor = sum;
}