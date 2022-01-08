#version 330

uniform mat4 Jeepcombined_xform;
uniform mat4 Jeepmodel_xform;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec2 vertexCoords;

out vec2 varyingCoords;
out vec3 VaryingPosition;

void main(void)
{	
	VaryingPosition = (Jeepmodel_xform * vec4(vertex_position, 1.0)).xyz;

	varyingCoords = vertexCoords;

	gl_Position = Jeepcombined_xform * Jeepmodel_xform * vec4(vertex_position, 1.0);
}