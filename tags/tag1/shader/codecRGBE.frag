
vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 255.0 - 128.0);
	return color * exponent;
}

vec4 encodeRGBE(vec4 color)
{
	float maxChannel = max(max(color.r, color.g), color.b);
	float exponent = ceil( log2(maxChannel * 255.0) );
	color.rgb /= exp2( exponent );
	color.a = (exponent + 128.0) / 255.0;

	return color;
}
