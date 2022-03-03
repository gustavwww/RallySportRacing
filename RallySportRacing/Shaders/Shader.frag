#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

// Materials.

// Envoirment.

// Light source.
uniform vec3 viewSpaceLightPos;
uniform vec3 lightColor;

//Inputs from vertex shader.
in vec3 color;
in vec2 texCoord;
in vec3 vertexPosition_viewspace;
in vec3 normal_viewspace;
in vec3 cameraDirection_viewspace;

// Outputs.
out vec3 fragmentColor;

void main(){

	//Normalize direction vectors and normals.
	vec3 normal = normalize(normal_viewspace);
	vec3 lightDir = normalize(viewSpaceLightPos - vertexPosition_viewspace);
	vec3 cameraDir = normalize(cameraDirection_viewspace);
	vec3 reflectDir = reflect(-lightDir, normal);

	float d = distance(viewSpaceLightPos, vertexPosition_viewspace);
	//Diffuse
	float l =  500 /(d*d);
	float diff = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = diff * lightColor * l;
	
	//Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	//Specular
	float specularStrength = 0;
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	fragmentColor = (diffuse + ambient + specular) * color;
}


