#version 150

in vec3 in_Color;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(in_Color, 0.5);
}
