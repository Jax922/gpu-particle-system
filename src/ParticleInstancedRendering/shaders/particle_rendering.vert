#version 460 core

// layout (location = 0) in vec4 ParticlePosition;
// layout (location = 1) in vec4 ParticleVelocity;
// layout (location = 2) in vec4 ParticleColor;

out vec4 Color;
out vec3 WorldPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

// The size of particles
uniform int ParticleSize;


layout(std430, binding = 0) buffer Pos {
	vec4 Positions[];
};

layout(std430, binding = 2) buffer Col {
	vec4 Colors[];
};

void main()
{

	gl_PointSize = ParticleSize;

	// Get Position and Color by Instance-ID
	int instanceID = gl_InstanceID;
	vec4 position = Positions[instanceID];
    Color = Colors[instanceID];

	WorldPosition = (WorldMatrix * vec4(position.xyz, 1.0)).xyz;
	gl_Position = ViewProjMatrix  * vec4(WorldPosition.xyz, 1.0);
}
