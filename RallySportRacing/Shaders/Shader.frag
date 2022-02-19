#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

//Inputs from vertex shader.
in vec3 fragmentColor;
in vec3 vertexPosition_viewspace;
//in vec3 normal_viewspace;

out vec3 color;

//Light source uniforms
uniform vec3 viewSpaceLightPos;
uniform vec3 lightColor;


void main(){

	color = fragmentColor;
	vec3 incomingLightDirection = normalize(viewSpaceLightPos - vertexPosition_viewspace); 
	//vec3 normal = normalize(normal_viewspace);

}



