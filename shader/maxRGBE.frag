uniform sampler2DRect inputTexture;

varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

/* ****************************** FUNCTIONS DECLARATIONS ***************************** */

vec4 decodeRGBE(vec4 color);
vec4 encodeRGBE(float luminance);

/* ***************************** END FUNCTIONS DECLARATIONS ************************** */

void main(void)
{
	// The values should be in the format (L, L, L, E) using RGBE encoding
	// Decode and then get Luminance (.x)
    float left		= decodeRGBE( texture2DRect(inputTexture, topLeft) ).x;
    float right		= decodeRGBE( texture2DRect(inputTexture, topRight) ).x;
    float top		= decodeRGBE( texture2DRect(inputTexture, bottomLeft) ).x;
    float bottom	= decodeRGBE( texture2DRect(inputTexture, bottomRight) ).x;
	
	float maxValue = max(max(left, right), max(top, bottom));

	gl_FragColor = encodeRGBE(maxValue);
}

/*
 * @description decode the color encoded with RGBE
 */
vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 255.0 - 128.0);
	return color * exponent;
}

/*
 * @param luminance it's the same for all channels,
 * so, it's considered as the max of all the channels
 * and, therefore, we only encode based on it
 */
vec4 encodeRGBE(float luminance)
{
	// NOTE: skipping ceil and the multiplication per 255.0 
	// because we have floats instead of integer
	float exponent = log2(luminance + 10e-8);
	float encL = luminance / exp2( exponent );
	float encE = (exponent + 128.0) / 255.0;

	return vec4(encL, encL, encL, encE);
}