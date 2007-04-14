uniform sampler2D irradianceMap;
varying vec3 R;
varying vec3 N;
varying vec3 V;
varying float Kd;

void main(void)
{
	N = normalize(gl_Normal);

	// View vector
	V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	V = V * gl_NormalMatrix;

	// using the incident view vector
	R = reflect(V, N);

	/*** OPTIMIZE: THE reflect can be computed by reusing the dot here below **/
	Kd = dot(-V, N);

	gl_Position = ftransform();
}