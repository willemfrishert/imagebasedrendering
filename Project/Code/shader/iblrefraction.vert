varying vec3 refraction;
varying vec3 reflection;

void myRefract(in vec3 incom, in vec3 normal, in float index_external, 
			   in float index_internal, out vec3 reflection, 
			   out vec3 refraction, out float reflectance, 
			   out float transmittance);

void main(void)
{
	vec3 N = normalize(gl_Normal);
	vec3 V = (gl_ModelViewMatrix * gl_Vertex).xyz;
	V = V * gl_NormalMatrix;
	V = normalize(V);

	//R = refract(V, N, 1.0/1.5);
	vec3 reflect;
	float reflectance;
	float transmittance;
	myRefract(V, N, 1.5, 1.0, reflection, refraction, reflectance, transmittance);

	gl_Position = ftransform();
}

void myRefract(in vec3 incom, in vec3 normal, in float index_external, 
			   in float index_internal, out vec3 reflection, 
			   out vec3 refraction, out float reflectance, 
			   out float transmittance)
{ 
	float eta = index_external/index_internal; 
	float cos_theta1 = dot(incom, normal); 
	float cos_theta2 = sqrt(1.0 - ((eta * eta) * ( 1.0 - (cos_theta1 * cos_theta1)))); 
	
	reflection = incom - 2.0 * cos_theta1 * normal; 
	refraction = (eta * incom) + (cos_theta2 - eta * cos_theta1) * normal; 
	
	float fresnel_rs = (index_external * cos_theta1 - index_internal * cos_theta2 ) / 
		(index_external * cos_theta1 + index_internal * cos_theta2); 
	float fresnel_rp = (index_internal * cos_theta1 - index_external * cos_theta2 ) / 
		(index_internal * cos_theta1 + index_external * cos_theta2); 
	
	reflectance = (fresnel_rs * fresnel_rs + fresnel_rp * fresnel_rp) / 2.0; 
	transmittance =((1.0-fresnel_rs) * (1.0-fresnel_rs) + (1.0-fresnel_rp) * (1.0-fresnel_rp)) / 2.0; 
}