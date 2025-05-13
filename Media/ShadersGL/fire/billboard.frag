
#version 440

// layout(binding = 0) uniform sampler3D sampler0;
// layout(binding = 1) uniform sampler2D sampler1;
uniform sampler2D tex0;
in vec3 tex;

layout(location = 0) out vec4 my_FragColor0;
vec4 color;
void main()
{
	color = texture(tex0, tex.xy);
	// if(color.a <= 1.0)
	// 	discard;
	if(color.r <= 0.2)
	 	discard;
	my_FragColor0 = color*(1-tex.z);
	my_FragColor0.a = 1-tex.z*0.5;
}
