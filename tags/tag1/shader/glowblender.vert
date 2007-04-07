void main() 
{	
	//Put the color in a varying variable
	gl_FrontColor = gl_Color;
	
	//Put the vertex in the position passed
	gl_Position = ftransform(); 
	
	//Use the first set of texture coordinates in the fragment shader 
	gl_TexCoord[0] = gl_MultiTexCoord0;
}