#version 330

//uniform sampler2D sampler_tex;

uniform samplerCube skybox;

in vec3 varyingCoords;
out vec4 fragment_colour;

void main(void)
{
	fragment_colour = texture(skybox, varyingCoords);
}