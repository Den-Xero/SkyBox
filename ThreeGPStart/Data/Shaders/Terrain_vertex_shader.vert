#version 330

uniform mat4 Terraincombined_xform;
uniform mat4 Terrainmodel_xform;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec2 vertex_coords;
layout (location=2) in vec3 vertex_normals;

out vec2 VaryingCoords;
out vec3 VaryingPosition;
out vec3 VaryingNormals;

void main(void)
{	
	VaryingPosition = (Terrainmodel_xform * vec4(vertex_position, 1.0)).xyz;

	VaryingNormals = mat3(Terrainmodel_xform) * vertex_normals;

	VaryingCoords = vertex_coords;

	gl_Position = Terraincombined_xform * Terrainmodel_xform * vec4(vertex_position, 1.0);
}