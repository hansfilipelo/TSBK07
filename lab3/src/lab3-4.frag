#version 150

in vec4 shading;

uniform sampler2D tex;

out vec4 out_Color;

void main(void)
{
	out_Color = shading;
}
