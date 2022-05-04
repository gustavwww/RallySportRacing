#version 420

layout(binding = 16) uniform sampler2D envTexture;

//Inputs
in vec2 pos;

//Outputs
out vec4 fragmentColor;

//Constants
const float PI = 3.14159265359;

void main(){

	//2D space to 3D space
	vec2 tp = (pos + 1) / 2;
	tp.y = 1 - tp.y;
	float ncosT = cos(tp.y * PI);
	float nsinT = sin(tp.y * PI);
	float ncosP = cos(tp.x * 2 * PI);
	float nsinP = sin(tp.x * 2 * PI);

	vec3 normal = normalize(vec3(nsinT * ncosP, ncosT, nsinT * nsinP));
	
	//Irradiance color.
	vec3 irradiance = vec3(0.0);
	
	//Create sampling tangent space.
	vec3 tg = cross(vec3(0.0, 1.0, 0.0), normal);
	vec3 bitg = cross(normal, tg);

	float sampleStep = 0.025;
	float nrSamples = 0.0;

	//Sampling
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleStep){
			
		float cosPhi = cos(phi);
		float sinPhi = sin(phi);

		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleStep){

			float cosTheta = cos(theta);
			float sinTheta = sin(theta);

			//Convertion from spherical to cartesian.
			vec3 tangentSample = vec3(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
			
			//Tangent space to world space direction vector.
			vec3 worldSample = tangentSample.x * tg + tangentSample.y * normal + tangentSample.z * bitg;

			float thetaWorld = acos(worldSample.y);
			float phiWorld = atan(worldSample.z, worldSample.x);

			vec2 lookup = vec2(phiWorld / (2 * PI), thetaWorld / PI);

			irradiance += texture(envTexture, lookup).rgb * cosTheta;
			nrSamples++;
		}
	}

	//Multiplacation by PI compensate weight issue.
	vec3 color = irradiance * PI * float((1.0 / nrSamples));

	fragmentColor = vec4(color, 1.0);
}