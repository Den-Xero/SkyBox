#version 330

uniform mat4 Cubecombined_xform;
uniform mat4 Cubemodel_xform;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_colour;

out vec3 varying_colour;

void main(void)
{	
	varying_colour = vertex_colour;

	gl_Position = Cubecombined_xform * Cubemodel_xform * vec4(vertex_position, 1.0);
}