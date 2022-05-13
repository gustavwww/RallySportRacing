#version 420 compatibility

precision highp float;

in float life;
uniform sampler2D textureIn;

void main()
{
	//Texture.
	gl_FragColor = texture2D(textureIn, gl_PointCoord);
	
	//Fade based on life time.
	gl_FragColor.xyz *= (1.0 - life);
	gl_FragColor.w = gl_FragColor.w * (1.0 - pow(life, 4.0)) * 0.05;
}