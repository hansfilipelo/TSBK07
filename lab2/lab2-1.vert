#version 150

in vec3 in_Normal;
in vec3 in_Position;
in vec2 inTexCoord;

uniform mat4 translMatrix;
uniform mat4 rotMatrix;
uniform mat4 rotMatrixX;

out outTexCoord;
out vec3 normal;

void main(void)
{
	gl_Position = rotMatrixX * rotMatrix * vec4(in_Position, 1.0);

	normal = in_Normal;
	outTexcoord = inTexCoord;
}
