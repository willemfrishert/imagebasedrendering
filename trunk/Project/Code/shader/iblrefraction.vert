varying vec3 refraction;
varying vec3 reflection;
varying float reflectance;
varying float transmittance;
uniform float eta1;
uniform float eta2;

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

	//R = refract(V, N, 1.0/1.5);
	vec3 reflect;
	fresnelRefraction(V, N, eta1, eta2, reflection, refraction, reflectance, transmittance);

	gl_Position = ftransform();
}

void fresnelRefraction(in vec3 V, in vec3 N, in float externalEta, 
			   in float internalEta, out vec3 reflection, 
			   out vec3 refraction, out float reflectance, 
			   out float transmittance)
{ 
	float eta = externalEta/internalEta; 
	float cosTheta1 = dot(V, N); 
	float cosTheta2 = sqrt(1.0 - ((eta * eta) * ( 1.0 - (cosTheta1 * cosTheta1)))); 
	
	reflection = V - 2.0 * cosTheta1 * N; 
	refraction = (eta * V) + (cosTheta2 - eta * cosTheta1) * N; 
	
	float Rs = (externalEta * cosTheta1 - internalEta * cosTheta2 ) / 
		(externalEta * cosTheta1 + internalEta * cosTheta2); 
	
	float Rp = (internalEta * cosTheta1 - externalEta * cosTheta2 ) / 
		(internalEta * cosTheta1 + externalEta * cosTheta2); 
	
	reflectance = (Rs * Rs + Rp * Rp) / 2.0; 
	transmittance =((1.0-Rs) * (1.0-Rs) + (1.0-Rp) * (1.0-Rp)) / 2.0; 
}