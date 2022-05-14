#version 420
layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 texCoord;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D baseColorTexture;
uniform unsigned int useBaseColorTexture;



void main(){
	
	/* Attempt to fix shadows for leaf.
	if(useBaseColorTexture == 0){
		gl_Position = (projection * view * instanceMatrix) * vec4(pos, 1.0);
	}else{
			vec4 color = texture(baseColorTexture, texCoord);
			if(color.a < 0.12){
				gl_Position = vec4(1.0);
			}else{
				gl_Position = (projection * view * instanceMatrix) * vec4(pos, 1.0);
			}
		}
	*/
	gl_Position = (projection * view * instanceMatrix) * vec4(pos, 1.0);
}