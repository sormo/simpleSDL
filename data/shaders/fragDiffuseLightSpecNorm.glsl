#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;
varying vec3 positionWorldSpace;
varying mat3 TBN;

uniform sampler2D textureDiffuse0;
uniform sampler2D textureNormal0;
uniform vec3 lightWorldSpace;
uniform vec3 cameraWorldSpace;

void main(void)
{
	float ambientStrength = 0.1;
	float specularStrength = 0.2;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 normalTangentSpace = texture2D(textureNormal0, vertexUVA).rgb;
	normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);   
	vec3 normalWorldSpace = normalize(TBN * normalTangentSpace); 

	vec3 fragmentColor = texture2D(textureDiffuse0, vertexUVA).rgb;
	// ambient
    vec3 ambient = ambientStrength * fragmentColor;
    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(lightWorldSpace - positionWorldSpace);  
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = diffuseFactor * lightColor * fragmentColor;
	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalWorldSpace);
	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), 30.0);
	vec3 specular = specularStrength * specularFactor * lightColor; 

    vec3 result = (ambient + diffuse + specular);

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
