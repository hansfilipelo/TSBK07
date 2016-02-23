#version 150

in vec3 in_Normal;
in vec3 in_Position;

uniform vec3 camera_position;
uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lookAtMatrix;

// Shading stuff
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

out vec3 shading;

void main(void)
{
	gl_Position = projectionMatrix * lookAtMatrix * transformMatrix * vec4(in_Position, 1.0);

	float diffuse, specular, shade;

	// Diffuse
	float temp_diffuse = 0;
	diffuse = 0;
	for(int i = 0; i < 4; i++)
	{
		temp_diffuse = dot(in_Normal, lightSourcesColorArr[i]);
		temp_diffuse = max(0.0, temp_diffuse); // No negative light
		diffuse += temp_diffuse;
	}

	// Specular
	float temp_specular = 0;
	specular = 0;
	for(int i = 0; i < 4; i++)
	{
		vec3 r;

		if (isDirectional[i])
		{
			r = reflect(lightSourcesDirPosArr[i], in_Normal);
		}
		else{
			r = reflect(normalize(lightSourcesDirPosArr[i]-in_Position), in_Normal);
		}

		vec3 v = normalize(camera_position-in_Position); // Reverse view direction

		temp_specular = dot(r, v);
		if (temp_specular > 0.0)
			temp_specular = 1.0 * pow(specular, specularExponent[i]);
		temp_specular = max(specular, 0.0);
		specular += temp_specular;
	}

	// Out
	shade = clamp(0.7*diffuse + 1*specular, 0, 1);
	shading = vec3(shade, shade, shade);
}
