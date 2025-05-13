
#version 440

layout(points) in;
layout(points, max_vertices = 80) out;

in vec4 vs_particlePos[];
in vec4 vs_particleVel[];
in vec4 vs_particleColor[];

layout(xfb_buffer = 0, xfb_stride = 48) out GS_OUTPUT {
	layout(xfb_offset = 0) vec4 particlePos;
	layout(xfb_offset = 16) vec4 particleVel;
	layout(xfb_offset = 32) vec4 particleColor;
} my_out;

uniform sampler2D randomTex;
layout(location = 1) uniform float time;
layout(location = 2) uniform float emitRate;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
vec3 randomVector(float xi) {
	return texture(randomTex, vec2(xi, 0.5)).xyz - vec3(0.5);
};

float randomScalar(float xi) {
	return 2.0 * texture(randomTex, vec2(xi, 0.0)).x - 1.0;
};
vec3 randomScalar3(float xi) {
	return 2.0 * texture(randomTex, vec2(xi, 0.0)).xyz - 1.0;
};

float randomFloat(float xi) {
	return texture(randomTex, vec2(xi, 0.5)).x;
};

void main()
{
	float seed = (time * 123525.0 + gl_PrimitiveIDIn * 1111.0) / 1234.0;
	float age = vs_particleVel[0].w;
	float vx = 0.05;
	float vz = 0.05;
	if (age >= emitRate) {
		for (int i = 0; i < 80; ++i) {
			vec3 vel;
			vec3 pos;
			vel.x = vx * randomScalar(seed);
			vel.y = 0.4 + randomFloat(seed)*0.05;
			vel.z = vz * randomScalar(seed + 4207.56);

			my_out.particlePos.xyz	= vs_particlePos[0].xyz + 0.5 * randomScalar3(seed);// + 0.5*vec3(rand(vec2(seed)),rand(vec2(seed+5)),rand(vec2(seed+10)));
			my_out.particlePos.w	= 1.0;
			my_out.particleVel.xyz	= vec3(0.0,vel.y,0.0);
			my_out.particleVel.w	= 0.0;
			my_out.particleColor	= vec4(randomScalar3(seed),randomFloat(seed));
			EmitVertex();
			seed += 4207.56;
		}
	}
}
