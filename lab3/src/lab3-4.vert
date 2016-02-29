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

out vec4 shading;

void main(void)
{
	vec3 surface_position = (transformMatrix * vec4(in_Position, 1.0)).xyz;
	vec3 world_normal = normalize(mat3(transformMatrix) * in_Normal);
	gl_Position = projectionMatrix * lookAtMatrix * transformMatrix * vec4(in_Position, 1.0);

	vec3 shade = vec3(0,0,0);
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);

	// Diffuse
	float temp_diffuse = 0;
	for(int i = 0; i < 4; i++)
	{
		temp_diffuse = dot(world_normal, lightSourcesDirPosArr[i]);
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
			r = reflect(lightSourcesDirPosArr[i], world_normal);
		}
		else{
			r = reflect(normalize(surface_position-lightSourcesDirPosArr[i]), world_normal);
		}

		vec3 v = normalize(camera_position-surface_position); // Reverse view direction

		temp_specular = dot(r, v);
		if (temp_specular > 0.0)
		{
			temp_specular = pow(temp_specular, specularExponent[i]);
		}
		temp_specular = max(temp_specular, 0.0);
		specular += temp_specular * lightSourcesColorArr[i];
	}

	// Out
	shade = specular;
	shading = vec4(shade, 1.0);
}
