uniform sampler2D blurTex;
uniform sampler2D originalTex;

void main(void)
{
	vec4 originalColor = texture2D(originalTex, gl_TexCoord[0].xy);
	//originalColor *= originalColor.a;
	
	gl_FragColor = texture2D(blurTex, gl_TexCoord[0].xy, 0.0) + originalColor;
}