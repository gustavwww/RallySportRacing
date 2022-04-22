#version 420
layout (location = 0) in vec3 pos;

//Output.
out vec3 pos_worldspace;

//Uniforms.
uniform mat4 viewMat;
uniform mat4 projMat;

void main(){
	pos_worldspace = pos;
	gl_Position = projMat * viewMat * vec4(pos_worldspace, 1.0);
}