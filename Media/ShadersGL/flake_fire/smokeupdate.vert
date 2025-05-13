
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
	float age = (particleVel.w/particleLife);
	vec3 Pos = particlePos.xyz;
	if(age <= 0.7){
		Pos.y = particlePos.y + vel.y * elapsedTime;
	}
	else if(age > 0.7 && age <= 0.9){
		float angle = particleColor.x * 6.28;
		float phi  = particleColor.z * 3.14;
		vel.x = 2*cos(angle);
		vel.y = 3*(particleColor.y - 0.5);
		vel.z = 2*sin(angle);
		Pos = Pos + vel * elapsedTime;
	}
	else if(age > 0.9){
		float angle = particleColor.x * 6.28;
		float phi  = particleColor.z * 3.14;
		vel.x = 2*cos(angle);
		vel.y += -age*0.1;
		vel.z = 2*sin(angle);
		Pos = Pos + vel * elapsedTime;
	}
	vs_particlePos.xyz  = Pos;
	vs_particlePos.w	= 1.0;
	vs_particleVel.xyz	= vel; //+ 0.01 * randomScalar3(particlePos.x * time);
	vs_particleVel.w	= particleVel.w + elapsedTime;
	vs_particleColor	= particleColor;
}
