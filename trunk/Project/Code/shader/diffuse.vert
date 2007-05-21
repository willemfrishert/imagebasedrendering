varying vec3 N;
varying vec3 V;
varying float ratio;

void main(void)
{
	N = normalize(gl_Normal);

	// View vector
	V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	V = V * gl_NormalMatrix;
	V = normalize(V);

	gl_Position = ftransform();
}