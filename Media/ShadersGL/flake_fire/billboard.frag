
#version 440

// layout(binding = 0) uniform sampler3D sampler0;
// layout(binding = 1) uniform sampler2D sampler1;
uniform sampler2D tex0;
in vec3 tex;
in vec3 color_rand;

layout(location = 0) out vec4 my_FragColor0;
vec4 color;
void main()
{
	color = texture(tex0, tex.xy);
	if(color.a == 0.0)
	 	discard;
	if(color.rgb == vec3(0,0,0))
	 	discard;
	my_FragColor0.rgb = 0.5*color.rgb + 0.5*color_rand*(1-tex.z + 0.8);
	my_FragColor0.a = 1 - tex.z*tex.z*0.6;
}
