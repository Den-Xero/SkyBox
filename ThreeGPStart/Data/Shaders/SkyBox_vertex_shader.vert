#version 330

uniform mat4 combined_xform2;
uniform mat4 model_xform;

layout (location=0) in vec3 vertex_position;


out vec3 varyingCoords;

void main(void)
{	
	varyingCoords = vertex_position * vec3(1,-1,1);

	gl_Position = combined_xform2 * model_xform * vec4(vertex_position, 1.0);
}