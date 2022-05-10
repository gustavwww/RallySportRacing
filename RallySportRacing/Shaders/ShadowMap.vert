#version 420
layout (location = 0) in vec3 pos;

uniform mat4 MVP;

void main(){
	gl_Position = MVP * vec4(pos, 1.0);
}