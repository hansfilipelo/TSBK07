#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 position;

// Light
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];
uniform vec3 camera_position;

uniform sampler2D tex;

void main(void)
{
	vec3 shade = vec3(0,0,0);
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);

	// Diffuse
	float temp_diffuse = 0;
	for(int i = 0; i < 4; i++)
	{
		temp_diffuse = dot(normal, lightSourcesDirPosArr[i]);
		temp_diffuse = max(0.0, temp_diffuse); // No negative light
		diffuse += temp_diffuse * lightSourcesColorArr[i];
	}

	// Specular
	float temp_specular;
	for(int i = 0; i < 4; i++)
	{
		vec3 r;

		if (isDirectional[i])
		{
			r = reflect(lightSourcesDirPosArr[i], normal);
		}
		else
		{
			r = reflect(normalize(position-lightSourcesDirPosArr[i]), normal);
		}

		vec3 v = normalize(camera_position-position); // Reverse view direction

		temp_specular = dot(r, v);
		if (temp_specular > 0.0)
		{
			temp_specular = pow(temp_specular, specularExponent[i]);
		}
		temp_specular = max(temp_specular, 0.0);
		specular += temp_specular * lightSourcesColorArr[i];
	}

	// Out
	shade = 0.05*diffuse + 0.1*specular; //vec4(shade, 1.0)
	outColor = vec4(shade, 1.0);// + texture(tex, texCoord);
}
