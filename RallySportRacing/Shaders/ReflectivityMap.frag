//Source code https://gitlab.com/ror3d/spherical-gaussian-lightfields/-/blob/main/Project/sg_renderer.glsl 

#version 450 compatibility
	const float PI = 3.14159265359;

	in vec2 pos;

	layout( binding = 16 ) uniform sampler2D env_map;
	uniform vec3 env_map_multiplier;

	uniform float roughness;
	uniform float max_envmap_value; 

	float numberOfSamples = 100;

	const vec3 perp( const vec3 a )
	{
		vec3 r = cross( a, vec3( 0, 0, 1 ) );
		if ( dot( r, r ) < 0.0001 )
		{
			r = cross( a, vec3( 0, 1, 0 ) );
		}
		return r;
	}

	unsigned int tea16( unsigned int val0, unsigned int val1 )
	{
		unsigned int v0 = val0;
		unsigned int v1 = val1;
		unsigned int s0 = 0;

		for ( unsigned int n = 0; n < 16; n++ )
		{
			s0 += 0x9e3779b9;
			v0 += ( ( v1 << 4 ) + 0xa341316c ) ^ ( v1 + s0 ) ^ ( ( v1 >> 5 ) + 0xc8013ea4 );
			v1 += ( ( v0 << 4 ) + 0xad90777d ) ^ ( v0 + s0 ) ^ ( ( v0 >> 5 ) + 0x7e95761e );
		}

		return v0;
	}

	unsigned int lcg( inout unsigned int prev )
	{
		const unsigned int LCG_A = 1664525u;
		const unsigned int LCG_C = 1013904223u;
		prev = ( LCG_A * prev + LCG_C );
		return prev & 0x00FFFFFF;
	}

	float randf_lcg( inout unsigned int prev )
	{
		return ( float( lcg( prev ) ) / float( 0x01000000 ) );
	}


	void main()
	{
		unsigned int seed = tea16( ( uint( 65537 * pos.x )  + 0x9E3779B9 + uint( 11579 * roughness ) ) * 97,
		                           ( uint( 65537 * pos.y )  + 0x9E3779B9 + uint( 13417 * ( 1 - roughness ) ) ) * 83 );

		float theta = pos.y * PI;
		float phi = pos.x * 2.f * PI;
		vec3 n = { sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) };
		vec3 t = normalize( perp( n ) );
		vec3 b = cross( n, t );

		// Sample GGX lobe around normal
		// NOTE: Code is from GGX importance sampling, for special case of normal incidence
		vec3 sum = vec3( 0.f );
		for ( int s = 0; s < numberOfSamples; s++ )
		{
			float alpha = roughness * roughness;
			float u1 = randf_lcg( seed );
			float u2 = randf_lcg( seed );
			float r = sqrt( u1 / ( 1 - u1 ) );
			float _phi = 2.f * PI * u2;
			vec2 slope = r * vec2( cos( _phi ), sin( _phi ) );
			slope *= alpha;
			vec3 wh = normalize( vec3( -slope.x, -slope.y, 1.0f ) );

			vec3 sdir = -reflect(vec3(0.0, 0.0, 1.0), wh);

			// Out of tangent space
			sdir = sdir.x * t + sdir.y * b + sdir.z * n;
			float Theta = acos( sdir.z );
			float Phi = atan( sdir.y, sdir.x );
			if ( Phi < 0.0f )
				Phi += 2.f * PI;

			// Sample image
			vec2 sUV = vec2( Phi / ( 2.f * PI ), Theta / PI );

			vec4 sv = texture2D( env_map, sUV );
			sum += clamp( env_map_multiplier * sv.xyz, 0.0, max_envmap_value > 0 ? max_envmap_value : 10000000.0);
		}
		gl_FragColor.rgb = sum / float(numberOfSamples );
	}