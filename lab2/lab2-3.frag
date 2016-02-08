#version 150

in vec3 normal;
in vec2 outTexCoord;

uniform sampler2D tex;
uniform float time_variable;

out vec4 out_Color;

void main(void)
{
	vec2 something = outTexCoord * time_variable; // Just scales the mapping coordinates
	out_Color = texture(tex, something);
}
