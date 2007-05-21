uniform float f;
uniform float fresnelPower;

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

	// Compute the ratio created by Christophe Schlick
	// See OpenGL Shading Language, Second Edition (orange book) page 337
	float cosTheta = dot(-V, N);
	ratio = f + (1.0 - f) * pow(1.0 - cosTheta, fresnelPower);

	gl_Position = ftransform();
}