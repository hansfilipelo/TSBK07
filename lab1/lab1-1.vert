#version 150

in vec3 in_Position;
in vec4 Color;

out vec4 in_Color;
void main(void)
{
	gl_Position = vec4(in_Position, 1.0);

	in_Color = Color;
}
