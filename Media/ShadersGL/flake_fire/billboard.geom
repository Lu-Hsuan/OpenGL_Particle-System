
#version 440

layout(location = 0) uniform mat4 matWVP;
layout(location = 1) uniform mat4 matWorldView;
layout(location = 2) uniform vec2 clipRadius;
layout(location = 3) uniform float particleLife;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vs_particlePos[];
in vec4 vs_particleVel[];
in vec4 vs_particleColor[];

out gl_PerVertex {
	vec4 gl_Position;
};

out vec3 tex;
out vec3 color_rand;
void main()
{
	vec4 pos = matWorldView * vs_particlePos[0];
	//vec4 vpos = matWorldView * vs_particlePos[0];

	float age = vs_particleVel[0].w;
	float normage = age / particleLife;
	float size = 0.03;
	if( normage > 0.7 && normage <= 0.9){
		size = 0.1 - normage*0.02;
	}
	else if(normage > 0.9){
		size = 0.1 - normage*0.05;
	}
	gl_Position = matWVP * (pos+vec4(-0.5,-0.5,0.0,0.0)*size);
	tex = vec3(0.0, 0.0, normage);
	color_rand = vs_particleColor[0].rgb;
	EmitVertex();

	gl_Position = matWVP * (pos+vec4(0.5,-0.5,0.0,0.0)*size);
	tex = vec3(1.0, 0.0, normage);
	color_rand = vs_particleColor[0].rgb;
	EmitVertex();

	gl_Position = matWVP * (pos+vec4(-0.5,+0.5,0.0,0.0)*size);
	tex = vec3(0.0, 1.0, normage);
	color_rand = vs_particleColor[0].rgb;
	EmitVertex();

	gl_Position = matWVP * (pos+vec4(0.5,0.5,0.0,0.0)*size);
	tex = vec3(1.0, 1.0, normage);
	color_rand = vs_particleColor[0].rgb;
	EmitVertex();
}
