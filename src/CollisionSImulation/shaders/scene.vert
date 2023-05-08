//Inputs
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

//Outputs
out vec3 WorldPosition;
out vec3 WorldNormal;

//Uniforms
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main()
{
	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
	WorldNormal = (WorldMatrix * vec4(VertexNormal, 0.0)).xyz;

	gl_Position =  ViewProjMatrix * vec4(WorldPosition, 1.0);
}
