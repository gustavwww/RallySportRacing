#version 420

//Inputs
layout(location = 0) in vec3 pos_worldSpace;

//Outputs
out vec3 TexCoords;

//Uniforms
uniform mat4 MVP;
uniform mat4 modelMat;

void main(){
	//Convert to modelSpace and then to vec3 TexCoords.
	TexCoords = vec3(modelMat * vec4(pos_worldSpace, 1.0));
	
	//Convert to MVP space.
	vec4 pos = MVP * vec4(pos_worldSpace, 1.0);
	
	//Use w coord instead of z making sure that depth is at maximum rendering behind everything else.
	gl_Position = pos.xyww;

}