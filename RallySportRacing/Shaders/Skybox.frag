#version 420

layout(binding = 6) uniform sampler2D skybox;

//Inputs
in vec2 texCoord;

//Outputs
out vec4 fragmentColor;

//Uniforms
uniform vec3 cameraPos;
uniform float envMultiplier;
uniform mat4 invPV;

//Constants
const float PI = 3.14159265359;

void main(){

	vec4 pixelPos = invPV * vec4(texCoord, 1.0, 1.0);
	pixelPos = (1.0 / pixelPos.w) * pixelPos;

	vec3 dir = normalize(pixelPos.xyz - cameraPos);

	float theta = acos(max(-1.0f, min(1.0f, dir.y)));
	float phi = atan(dir.z, dir.x);

	if(phi < 0.0f)
		phi = phi + 2.0f * PI;
	// Use these to lookup the color in the environment map
	vec2 lookup = vec2(phi / (2.0 * PI), theta / PI);
	fragmentColor = envMultiplier * texture(skybox, lookup);
}