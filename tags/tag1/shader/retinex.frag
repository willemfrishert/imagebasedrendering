uniform sampler2D HDRtexture;

vec3 decodeRGBE( vec4 encoded )
{
	vec2 scaleBias = vec2( 255.0, -128.0 );

	// decode the encoded value
	//allow the exponent to be negative [0.0, 1.0] -> [-128, 127]
	vec3 decoded = encoded.rgb * exp2( encoded.a * scaleBias.x + scaleBias.y );
	return decoded;
}

void main() 
{
	//Get the encoded HDR value of the texture
	vec4 encoded = texture2D(HDRtexture, gl_TexCoord[0].xy);
	// decode the encoded texel
	vec3 decoded = decodeRGBE( encoded );

	gl_FragColor = vec4(decoded, 1.0);

	// test to see whether the intensities are still in there
//	gl_FragColor = vec4(decoded*0.05, 1.0);
	//gl_FragColor = encoded;
}