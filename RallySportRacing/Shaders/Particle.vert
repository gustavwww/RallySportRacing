#version 420 compatibility
layout(location = 0) in vec4 particle;

uniform mat4 projMat;
uniform float screen_x;
uniform float screen_y;
uniform float scale;

out float life;

void main()
{
	//scaleTest = test; 
	life = particle.w;
	//Use 1 instead of lifetime when calculating fragment.
	vec4 particle_viewspace = vec4(particle.xyz, 1.0);
	
	vec4 projQuad = projMat * vec4(1.0, 1.0, particle_viewspace.z, particle_viewspace.w);
	vec2 projPixel = vec2(screen_x, screen_y) * projQuad.xy / projQuad.w;
	float scaleFactor = (projPixel.x + projPixel.y);
	gl_Position = projMat * particle_viewspace;

	//Scale size based on lifetime.
	gl_PointSize = scaleFactor * mix(0.0, scale, pow(life, 1.0 / 4.0)); // 
}