#version 420

//Inputs
in vec3 TexCoords;

//Outputs
out vec4 FragColor;

//Uniforms
uniform samplerCube background;

void main(){
	FragColor = texture(background, TexCoords);
}