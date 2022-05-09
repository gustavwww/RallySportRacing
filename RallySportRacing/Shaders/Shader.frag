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
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
layout(binding = 9) uniform sampler2DShadow shadowMap;
////////////////////////////////
//Shadow
////////////////////////////////

////////////////////////////////
// Light source
////////////////////////////////
uniform vec3 viewSpaceLightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

////////////////////////////////
// Constants
////////////////////////////////
#define PI 3.14159265359

////////////////////////////////
//Inputs from vertex shader
////////////////////////////////
in vec3 vertexPosition_viewspace;
in vec3 normal_viewspace;
in vec2 texCoord;
in vec4 shadowMapCoord;

////////////////////////////////
//Uniforms input
////////////////////////////////
uniform mat4 viewInverse;

////////////////////////////////
// Outputs
////////////////////////////////
out vec4 fragmentColor;

//Trowbridge-Reitz GGX normal distrubition function: a^2 / pi( (n dot h)^2 * (a^2 - 1) + 1 )^2
//Aproximates the relative surface area of microfacets aligned with the halfway vector.
float normaldistrubutionGGX(float NdotH, float roughness){
	
	float alpha_Squared = roughness * roughness;
	float alpha_Squared2 = alpha_Squared * alpha_Squared;
	float NdotH_Squared = NdotH * NdotH;
	
	float denom = NdotH_Squared * (alpha_Squared2 - 1) + 1;
	float denomenator = PI * denom * denom;

	return alpha_Squared2 / denomenator;
}

//Geometry function Schlick-GGX together with Smith's method. 
//Schlick-GGX remaps roughness to simulate direct illumination: (n dot v) / ( (n dot v) * (1 - k) + k )
//Smith's method result in a float between [0.0, 1.0] that describe the microfacet selfshadowing: Geometryfunction(n,v,k) * Geometryfunction(n,l,k).
float geometryFunction(float NdotV, float NdotL, float roughness){

	float kDirect = (roughness + 1) * (roughness + 1) / 8;
	float ggx1 = NdotV / (NdotV * (1.0 - kDirect) +  kDirect);
	float ggx2 = NdotL / (NdotL * (1.0 - kDirect) + kDirect);

	return ggx1 * ggx2;
}

//Fresnel-Schlick approximation: F0 + (1 - F0) * (1 - (h dot v))^5
//This returns a vector which gives the surface a fresnel affect based on the baseReflectivity of the surface.
vec3 fresnelSchlick(float HdotV, vec3 baseReflectivity){
	
	return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
}

vec3 fresnelSchlickRoughness(float NdotV, vec3 baseReflectivity, float roughness){
	
	return baseReflectivity + (max(vec3(1.0 - roughness), baseReflectivity) - baseReflectivity) * pow(1.0 - NdotV, 5.0);
}

void main(){

	/////////////////////////////////
	//NEW PBR PIPELINE
	/////////////////////////////////
	//Normalize direction vector and normal vector.
	vec3 pos = -normalize(vertexPosition_viewspace);
	vec3 normal = normalize(normal_viewspace);
	vec3 viewDir = normalize(vec3(0,0,0) - vertexPosition_viewspace);

	//If a dia-electric use baseReflectivity 0.04 and if metal use the albedo color as baseReflectivity.
	vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);

	//Luminance output.
	vec3 Lo = vec3(0.0f);
	
	///////////
	//ToDo add for loop here for multiple light sources to calculate radiance per-light.
	///////////
	{
		//Normalize direction vectors.
		vec3 lightDir = normalize(viewSpaceLightPos - vertexPosition_viewspace);
		vec3 halfwayVector = normalize(viewDir + lightDir);

		float d = distance(viewSpaceLightPos, vertexPosition_viewspace);
		float attenuation = 1.0/(d * d);
		vec3 radiance = lightColor * lightIntensity * attenuation;

		//Dot products.
		float NdotV = max(dot(normal, viewDir), 0.0000001);
		float NdotL = max(dot(normal, lightDir), 0.0000001);
		float NdotH = max(dot(normal, halfwayVector), 0.0);
		float HdotV = max(dot(halfwayVector, viewDir), 0.0);
		float LdotN = max(dot(lightDir, normal), 0.0);

		//Cook-Torrance BRDF: D*F*G /( 4 * (w0 dot n) * (wi dot n) )
		float D = normaldistrubutionGGX(NdotH, roughness + 0.01);
		float G = geometryFunction(NdotV, NdotL, roughness  + 0.01);
		vec3 F = fresnelSchlick(HdotV, baseReflectivity);

		vec3 specular = D * G * F;
		specular /= 4.0 * NdotV * NdotL;

		//Specular energy contribution.
		vec3 kSpec = F;
		//Diffuse energy contribution.
		vec3 kDiff = vec3(1.0) - kSpec;
		//Remove diffuse component if metal.
		kDiff *= 1.0 - metallic;
		vec3 light = (kDiff * albedo / PI + specular) * radiance * NdotL;
		float shadow = textureProj(shadowMap, shadowMapCoord);
		Lo += light * shadow;
	}
	//ToDo end for each light loop here.
	
	//Ambient light.
	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), baseReflectivity, roughness);
	vec3 kSpec = F;
	vec3 kDiff = 1.0 - kSpec;
	kDiff *= 1 - metallic;

	//Diffuse component.
	vec4 normalWorldSpace = viewInverse * vec4(normal, 0.0f);
	float theta = acos(max(-1.0f, min(1.0f, normalWorldSpace.y)));
	float phi = atan(normalWorldSpace.z, normalWorldSpace.x);
	if(phi < 0.0f)
	{
		phi = phi + 2.0f * PI;
	}

	vec2 lookup = vec2(phi / (2.0 * PI), theta / PI);

	vec3 irradiance = 0.5f * texture(irradianceMap, lookup).rgb;
	vec3 diffuseAmbient =  irradiance  * albedo * (1.0/PI) * kDiff;


	//Specular component.
	vec4 wiWorldSpace = normalize(viewInverse * vec4(-reflect(pos, normal), 0.0f));
	vec4 woWorldSpace = normalize(viewInverse * vec4(pos, 0.0f));

	const float MAX_REFLECTION_LOD = 4.0;
	
	theta = acos(max(-1.0f, min(1.0f, wiWorldSpace.y)));
	phi = atan(wiWorldSpace.z, wiWorldSpace.x);
	if(phi < 0.0f)
	{
		phi = phi + 2.0f * PI;
	}
	lookup = vec2(phi / (2.0 * PI), theta / PI);
	
	vec3 radiance = 0.5 * textureLod(reflectionMap, lookup, roughness * MAX_REFLECTION_LOD).rgb;
	//vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
	//vec3 specularAmbient = prefilteredColor * (F * brdf.r +brdf.g);
	vec3 specularAmbient = radiance * kSpec;

	vec3 ambient = (diffuseAmbient + specularAmbient);
	vec3 color =  Lo + ambient;

	//HDR tonemapping.
	color = color / (color + vec3(1.0));
	
	//Gamma correction.
	color = pow(color, vec3(1.0/2.2));

	fragmentColor = vec4(color, 1.0);
}