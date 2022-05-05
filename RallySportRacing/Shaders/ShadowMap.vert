#version 420
layout (location = 0) in vec3 pos;

uniform mat4 lightProjection;
uniform mat4 model;

void main(){
	gl_Position = lightProjection * model * vec4(pos, 1.0);
}