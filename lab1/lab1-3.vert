#version 150

in vec3 in_Position;

uniform mat4 translMatrix;
uniform mat4 rotMatrix;

void main(void)
{
	gl_Position = rotMatrix * translMatrix * vec4(in_Position, 1.0);

}
