#version 420

//Inputs
in vec3 pos;

//Output
out vec2 texCoord;

//Uniforms
uniform mat4 viewMat;
uniform mat4 projMat;

void main(){
	texCoord = pos.xy;
	gl_Position = projMat * viewMat * vec4(pos, 1.0);
}