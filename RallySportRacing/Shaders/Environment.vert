#version 420

//Inputs
layout (location = 0) in vec2 posIn;

//Output
out vec2 pos;

//Uniforms
uniform mat4 viewMat;
uniform mat4 projMat;

void main(){
	pos = posIn;
	//ToDo check if this row is necessaryneseceary.
	gl_Position = vec4(pos, 0, 1.0);
}