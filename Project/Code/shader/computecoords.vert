varying vec2 topLeft;
varying vec2 topRight;
varying vec2 bottomLeft;
varying vec2 bottomRight;

void main(void)
{
	// interpolate the coordinates for the fragment shader
	topLeft		= 2.0 * gl_Vertex.st - 0.5;
	topRight	= topLeft + vec2(1.0,0.0);
	bottomLeft	= topLeft + vec2(1.0,1.0);
	bottomRight = topLeft + vec2(0.0,1.0);

	gl_Position = ftransform();
}