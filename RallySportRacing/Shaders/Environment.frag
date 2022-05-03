#version 420

layout(binding = 16) uniform sampler2D envTexture;

//Inputs
in vec2 pos;

//Outputs
out vec4 fragmentColor;

//Constants
const float PI = 3.14159265359;

void main(){

	vec3 normal = normalize(vec3(pos, 0));
	
	//Irradiance color.
	vec3 irradiance = vec3(0.0);
	
	//Create sampling tangent space.
	vec3 right = cross(vec3(0.0, 1.0, 0.0), normal);
	vec3 up = cross(normal, right);

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
			vec3 tangentSample = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
			
			//Tangent space to world space conversion.
			vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			float thetaWorld = acos(worldSample.z);
			float phiWorld = atan(worldSample.y, worldSample.x);

			vec2 lookup = vec2(thetaWorld, phiWorld);

			irradiance += texture(envTexture, lookup).rgb * cosTheta * sinTheta;
			nrSamples++;
		}
	}

	//Multiplacation by PI compensate weight issue.
	vec3 color = irradiance * PI * float((1.0 / nrSamples));

	//Tonemapping
	//color = color / (color + vec3(1.0));
    
	//Gamma correction
	color = pow(color, vec3(1.0/2.2)); 

	fragmentColor = vec4(color, 1.0);
}