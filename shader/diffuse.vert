varying vec3 N;
varying float ratio;

void main(void)
{
	N = normalize(gl_Normal);

	gl_Position = ftransform();
}