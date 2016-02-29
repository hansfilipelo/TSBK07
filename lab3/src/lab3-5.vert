#version 150

in vec3 in_Normal;
in vec3 in_Position;
in vec2 inTexCoord;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;

out vec3 world_normal;
out vec3 surface_position;

out vec2 outTexCoord;

void main(void)
{
	surface_position = (transformMatrix * vec4(in_Position, 1.0)).xyz;
	world_normal = normalize(mat3(transformMatrix) * in_Normal);
	gl_Position = projectionMatrix * lookAtMatrix * transformMatrix * vec4(in_Position, 1.0);
	outTexCoord = inTexCoord;
}
