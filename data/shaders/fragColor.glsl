#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
}; 

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 cameraWorldSpace;

void main(void)
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;

    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);  
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = light.diffuse * (diffuseFactor * material.diffuse);

	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normalWorldSpace));
	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), material.shininess);
	vec3 specular = light.specular * (specularFactor * material.specular);

    vec3 result = ambient + diffuse + specular;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
