#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;
varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;

uniform sampler2D textureDiffuse0;
uniform vec3 lightWorldSpace;

void main(void)
{
	float ambientStrength = 0.1;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 fragmentColor = texture2D(textureDiffuse0, vertexUVA).rgb;
    vec3 ambient = ambientStrength * lightColor;

	vec3 ligthDirectionWorldSpace = normalize(lightWorldSpace - positionWorldSpace);  
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = diffuseFactor * lightColor;

    vec3 result = (ambient + diffuse) * fragmentColor;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
