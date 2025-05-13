
#version 440

layout(location = 0) in vec4 particlePos;
layout(location = 1) in vec4 particleVel;
layout(location = 2) in vec4 particleColor;

out vec4 vs_particlePos;
out vec4 vs_particleVel;
out vec4 vs_particleColor;

layout(location = 0) uniform float elapsedTime;
layout(location = 1) uniform float buoyancy;
layout(location = 2) uniform float particleLife;
layout(location = 3) uniform vec3 ori_position;
layout(location = 4) uniform float time;
uniform sampler2D randomTex;

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
	float seed = (time * 123525.0) / 1234.0;
	vec3 vel = particleVel.xyz;
	vec3 diff = particlePos.xyz - ori_position;
	float age = ((particleVel.w - 20*particleColor.w*elapsedTime)/particleLife);
	float life = (particleVel.w - 20*particleColor.w*elapsedTime);
	vs_particlePos.z = particlePos.z + vel.z * elapsedTime;
	
	vs_particlePos.x = 0.5*log(1+life)*sin(life * 3.14 * 0.7) + ori_position.x;// + sin(particleColor.x*6.28);
	vs_particlePos.y = 0.5*log(1+life)*cos(life * 3.14 * 0.7) + ori_position.y;// + cos(particleColor.x*6.28);
	vs_particlePos.w	= 1.0;
	vs_particleVel.xyz	= vel; //+ 0.01 * randomScalar3(particlePos.x * time);
	vs_particleVel.w	= particleVel.w + elapsedTime;
	vs_particleColor	= particleColor;
}
