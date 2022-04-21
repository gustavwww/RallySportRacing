#version 420

//Inputs
in vec3 TexCoords;

//Outputs
out vec4 fragmentColor;

//Uniforms
uniform samplerCube background;

void main(){
	fragmentColor = texture(background, TexCoords);
}