#version 150

in vec3 in_Normal;
in vec3 in_Position;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;

out vec3 shading;

void main(void)
{
	float shade;
	const vec3 light = vec3(0.58, 0.58, 0.58);

	gl_Position = projectionMatrix * lookAtMatrix * transformMatrix * vec4(in_Position, 1.0);

	vec3 temp_normal = mat3(lookAtMatrix) * in_Normal;
	shade = dot(normalize(temp_normal),light);
	shading = vec3(shade);
}
