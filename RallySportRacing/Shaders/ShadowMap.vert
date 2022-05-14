#version 420
layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 texCoordIn;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D baseColorTexture;
uniform unsigned int useBaseColorTexture;

out vec2 texCoord;



void main(){

	texCoord = texCoordIn;

	gl_Position = (projection * view * instanceMatrix) * vec4(pos, 1.0);
}