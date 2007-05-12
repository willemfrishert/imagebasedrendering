varying vec3 refraction;
varying vec3 reflection;
uniform float eta;
uniform float f;
uniform float fresnelPower;

varying float ratio;

void fresnelRefraction(in vec3 incom, in vec3 normal, in float externalEta, 
			   in float internalEta, out vec3 reflection, 
			   out vec3 refraction, out float reflectance, 
			   out float transmittance);

void main(void)
{
	vec3 N = normalize(gl_Normal);
	vec3 V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	V = V * gl_NormalMatrix;
	V = normalize(V);
	N = normalize(N);

	// Compute the ratio created by Christophe Schlick
	float cosTheta = dot(-V, N);
	ratio = f + (1.0 - f) * pow(1.0 - cosTheta, fresnelPower);

	// get the two vectors: reflection and refraction
	refraction = refract(V, N, eta);
	reflection = reflect(V, N);

	gl_Position = ftransform();
}