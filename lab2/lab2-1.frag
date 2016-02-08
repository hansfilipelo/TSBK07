#version 150

in vec3 normal;
in vec2 outTexCoord;

out vec4 out_Color;

void main(void)
{
	out_Color =  vec4(normal, 0.5);
}
