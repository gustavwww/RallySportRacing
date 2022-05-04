#version 420

//Inputs
layout(location = 0) in vec2 pos;

//Outputs
out vec2 texCoord;

void main(){
	
	gl_Position = vec4(pos, 0.0, 1.0);
	texCoord = pos;
}