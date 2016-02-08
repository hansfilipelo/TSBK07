#version 150

in vec3 in_Normal;
in vec3 in_Position;
in vec2 inTexCoord;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;

out vec2 outTexCoord;
out vec3 normal;

void main(void)
{
	gl_Position = projectionMatrix * transformMatrix * vec4(in_Position, 1.0);

	normal = in_Normal;
	outTexCoord = inTexCoord;
}
