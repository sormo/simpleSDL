#version 140

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;
varying vec3 positionWorldSpace;
varying vec3 lightTangentSpace;
varying vec3 cameraTangentSpace;
varying vec3 positionTangentSpace;

uniform sampler2D textureDiffuse0;
uniform sampler2D textureNormal0;
uniform vec3 lightWorldSpace;
uniform vec3 cameraWorldSpace;

void main(void)
{
	float ambientStrength = 0.1;
	float specularStrength = 1.0;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 normalTangentSpace = texture2D(textureNormal0, vertexUVA).rgb;
	normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);   

	vec3 fragmentColor = texture2D(textureDiffuse0, vertexUVA).rgb;
	// ambient
    vec3 ambient = ambientStrength * fragmentColor;
    // diffuse
	vec3 ligthDirectionTangentSpace = normalize(lightTangentSpace - positionTangentSpace);  
	float diffuseFactor = max(dot(normalTangentSpace, ligthDirectionTangentSpace), 0.0);
	vec3 diffuse = diffuseFactor * fragmentColor;
	// specular
	vec3 cameraDirectionTangentSpace = normalize(cameraTangentSpace - positionTangentSpace);
	//vec3 reflectDirectionTangentSpace = reflect(-ligthDirectionTangentSpace, normalTangentSpace);
	vec3 halfwayDirectionTangentSpace = normalize(ligthDirectionTangentSpace + cameraDirectionTangentSpace); 
	float specularFactor = pow(max(dot(normalTangentSpace, halfwayDirectionTangentSpace), 0.0), 32.0);
	vec3 specular = specularStrength * specularFactor * vec3(1.0, 0.0, 0.0); 

    vec3 result = (ambient + diffuse + specular);

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
