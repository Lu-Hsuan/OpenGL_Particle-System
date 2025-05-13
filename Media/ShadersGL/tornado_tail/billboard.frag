
#version 440

// layout(binding = 0) uniform sampler3D sampler0;
// layout(binding = 1) uniform sampler2D sampler1;
uniform sampler2D tex0;
in vec3 tex;
in vec4 color_rand;

layout(location = 0) out vec4 my_FragColor0;
vec4 color;
void main()
{
	color = texture(tex0, tex.xy);
	if(color.a == 0.0)
	 	discard;
	if(color.rgb == vec3(0,0,0))
	 	discard;
	if(color.r <= 0.5)
	 	discard;
	my_FragColor0 = color * (1.0-color_rand.w);
	my_FragColor0.a = 1.0-color_rand.w;
}
