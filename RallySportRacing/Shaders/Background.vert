#version 420

//Inputs
layout(location = 0) in vec3 pos;

//Outputs
out vec3 TexCoords;

//Uniforms
uniform mat4 viewProjMat;

void main(){
	//Convert to vec3 TexCoords.
	TexCoords = pos;
	
	//Convert to view projection space space.
	vec4 glPos = viewProjMat * vec4(pos, 1.0);
	
	//Use w coord instead of z making sure that depth is at maximum rendering behind everything else.
	gl_Position = glPos.xyww;

}