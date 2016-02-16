#version 150

in vec2 outTexCoord;
in vec3 shading;

uniform sampler2D tex;

out vec4 out_Color;

void main(void)
{
	out_Color = texture(tex, outTexCoord);
}
