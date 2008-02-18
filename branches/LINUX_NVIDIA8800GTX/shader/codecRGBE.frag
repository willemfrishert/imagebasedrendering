// coding / encoding RGBE

vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 256.0 - 128.0);
	return color * exponent;
}

vec4 encodeRGBE(in vec4 color)
{
	float maxChannel = max(max(color.r, color.g), color.b);
	float exponent = ceil( log2(max(maxChannel, 1e-8)) );
	color.rgb /= exp2( exponent );
	color.a = (exponent + 128.0) / 256.0;

	return color;
}