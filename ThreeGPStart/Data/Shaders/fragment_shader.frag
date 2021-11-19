#version 330

//uniform sampler2D sampler_tex;

uniform vec4 diffuse_colour;
uniform sampler2D SamplerTex;

in vec2 varyingCoords;

//vec3 N = normalize(varying_colour);

out vec4 fragment_colour;

void main(void)
{
	//fragment_colour = vec4(1.0,1.0,1.0,1.0);
	vec3 TexColour = texture(SamplerTex, varyingCoords).rgb;
	fragment_colour = vec4(TexColour,1.0);
}