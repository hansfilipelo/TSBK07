#version 150

in vec3 in_Position;
in vec3 Color;

uniform mat4 translMatrix;
uniform mat4 rotMatrix;
uniform mat4 rotMatrixX;

out vec3 in_Color;

void main(void)
{
	gl_Position = rotMatrix * rotMatrixX * vec4(in_Position, 1.0);

	in_Color = Color;
}
