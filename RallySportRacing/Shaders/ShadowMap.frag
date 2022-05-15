#version 420

precision highp float;

layout(location = 0) out vec4 fragmentColor;


layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 texCoord;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D baseColorTexture;
uniform unsigned int useBaseColorTexture;

void main(){
	
	//Attempt to fix shadows for leaf.
	if(useBaseColorTexture != 0){
		vec4 color = texture(baseColorTexture, texCoord);
		if(color.a < 0.5){
			discard;
		}
	}

	fragmentColor = vec4(gl_FragCoord.z);
}