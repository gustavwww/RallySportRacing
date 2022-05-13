#version 420
layout (location = 0) in vec3 pos;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = (projection * view * instanceMatrix) * vec4(pos, 1.0);
}