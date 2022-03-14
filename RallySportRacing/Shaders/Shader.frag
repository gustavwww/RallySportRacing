#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

////////////////////////////////
// Materials
////////////////////////////////
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
////////////////////////////////
// Envoirment
////////////////////////////////

////////////////////////////////
//Shadow
////////////////////////////////

////////////////////////////////
// Light source
////////////////////////////////
uniform vec3 viewSpaceLightPos;
uniform vec3 lightColor;

////////////////////////////////
// Constants
////////////////////////////////
#define PI 3.14159265359

////////////////////////////////
//Inputs from vertex shader
////////////////////////////////
in vec3 color;
in vec2 texCoord;
in vec3 vertexPosition_viewspace;
in vec3 normal_viewspace;
in vec3 cameraDirection_viewspace;

////////////////////////////////
// Outputs
////////////////////////////////
out vec3 fragmentColor;

//Trowbridge-Reitz GGX normal distrubition function: a^2 / pi( (n dot h)^2 * (a^2 - 1) + 1 )^2
//Aproximates the relative surface area of microfacets aligned with the halfway vector.
float normaldistrubutionGGX(vec3 normal, vec3 halfwayVector, float roughness){
	float alpha_Squared = roughness * roughness;
	float NdotH = max(dot(normal, halfwayVector), 0.0);
	float NdotH_Squared = NdotH * NdotH;
	
	float denom = NdotH_Squared * (alpha_Squared - 1) + 1;
	float denomenator = PI * denom * denom;

	return alpha_Squared / denomenator;
}

//Geometry function Schlick-GGX together with Smith's method. 
//Schlick-GGX remaps rougness to simulate direct illumination: (n dot v) / ( (n dot v) * (1 - k) + k )
//Smith's method result in a float between [0.0, 1.0] that describe the microfacet selfshadowing: Geometryfunction(n,v,k) * Geometryfunction(n,l,k).
float geometryFunction(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness){
	float NdotV = max(dot(normal, viewDirection), 0.0);
	float NdotL = max(dot(normal, lightDirection), 0.0);

	float kDirect = (roughness + 1) * (roughness + 1) / 8;
	float ggx1 = NdotV / (NdotV * (1.0 - kDirect) +  kDirect);
	float ggx2 = NdotL / (NdotL * (1.0 - kDirect) + kDirect);

	return ggx1 * ggx2;
}

//Fresnel-Schlick approximation: F0 + (1 - F0) * (1 - (h dot v))^5
//This returns a vector which gives the surface a fresnel affect based on the baseReflectivity of the surface.
vec3 fresnelSchlick(vec3 halfwayVector, vec3 viewDirection, vec3 baseReflectivity){
	
	float HdotV = max(dot(halfwayVector, viewDirection), 0.0);
	return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
}

void main(){

	//Normalize direction vectors and normals.
	vec3 normal = normalize(normal_viewspace);
	vec3 lightDir = normalize(viewSpaceLightPos - vertexPosition_viewspace);
	vec3 cameraDir = normalize(cameraDirection_viewspace);
	vec3 reflectDir = reflect(-lightDir, normal);

	float d = distance(viewSpaceLightPos, vertexPosition_viewspace);
	//Diffuse
	float diffuseStrength = 500;
	float diff = max(dot(normal,lightDir), 0.0) *  1/(d*d);
	vec3 diffuse = diff * diffuseStrength * lightColor;
	
	//Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	//Specular
	float specularStrength = 0;
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	fragmentColor = (diffuse + ambient + specular) * color;
}

