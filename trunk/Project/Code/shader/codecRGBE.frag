

vec4 decodeRGBE(vec4 color)
{
	float exponent = exp2(color.a * 256.0 - 128.0);
	return color * exponent;
}

vec4 encodeRGBE(in vec4 color)
{
	float maxChannel = max(max(color.r, color.g), color.b);
	float exponent = ceil( log2((maxChannel + 1e-8) * 256.0 ) );
	color.rgb /= exp2( exponent );
	color.a = (exponent + 128.0) / 256.0;

	return color;
}

//float invScale = 1.0/255.0;
//float invBias = 0.5;
//
//vec4 encodeRGBE(vec4 color)
//{
//   vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);
//   float maxChannel = max(max(color.r, color.g), color.b);
//
//   maxChannel = max(maxChannel, 1e-8);
//   finalColor.rgb /= maxChannel;
//   finalColor.a = log2(maxChannel) * invScale + invBias;
//   return finalColor;
//} 