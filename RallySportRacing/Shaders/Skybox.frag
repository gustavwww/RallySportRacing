#version 420

//Inputs
in vec3 TexCoords;

//Outputs
out vec4 fragmentColor;

//Uniforms
uniform samplerCube skybox;

void main(){
	vec3 envColor = texture(skybox, TexCoords).rgb;

	//HDR tone mapping.
	envColor = min(envColor / (envColor + vec3(1.0)), 10000);

	//Gamma correction.
    envColor = pow(envColor, vec3(1.0/2.2));
	

	fragmentColor = vec4(envColor, 1.0);
}