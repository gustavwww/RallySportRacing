#version 420

//Inputs.
in vec3 pos_worldspace;

//Output.
out vec4 fragmentColor;

//Uniforms.
uniform sampler2D hdrTexture;

//Inverste atan.
const vec2 invAtan = vec2(0.1591, 0.3183);

//Coverts pos to sphereMap texCoords.
vec2 sphereMapSampler(vec3 pos){
	vec2 texCoords = vec2(atan(pos.z, pos.x), asin(pos.y));
	texCoords *= invAtan;
	texCoords += 0.5;
	return texCoords;
}

void main(){
	vec2 mapCoords = sphereMapSampler(normalize(pos_worldspace));
	vec3 color = texture(hdrTexture, mapCoords).rgb;

	fragmentColor = vec4(color, 1.0);
}