#version 420

layout(binding = 0) uniform sampler2D tex;

//Inputs
in vec2 pos;

//Outputs
out vec4 fragmentColor;

void main(){

	fragmentColor = vec4(texture(tex, pos).rgb, 1.0);
}