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
    //float left		= texture2D(inputTexture, topLeft / height).y;
    //float right		= texture2D(inputTexture, topRight / height).y;
    //float top		= texture2D(inputTexture, bottomLeft / height).y;
    //float bottom	= texture2D(inputTexture, bottomRight / height).y;
	
	vec4 left		= texture2D(inputTexture, topLeft / height);
    vec4 right		= texture2D(inputTexture, topRight / height);
    vec4 top		= texture2D(inputTexture, bottomLeft / height);
    vec4 bottom		= texture2D(inputTexture, bottomRight / height);

	// sum the fetched log(L)
	float sum = left.y + right.y + bottom.y + top.y;
	
	// compute max and min and store them on the Z and W channels
	float maxL = max(max(left.z, right.z), max(top.z, bottom.z));
	float minL = min(min(left.w, right.w), min(top.w, bottom.w));

	/*gl_FragColor = vec4(sum, sum, sum, right);*/
	gl_FragColor = vec4(sum, sum, maxL, minL);
}