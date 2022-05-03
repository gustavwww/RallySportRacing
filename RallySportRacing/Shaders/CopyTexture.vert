#version 420

//Inputs
layout (location = 0) in vec2 posIn;

//Output
out vec2 pos;

void main(){
	pos = (posIn + 1) / 2;
	gl_Position = vec4(posIn, 0, 1.0);
}