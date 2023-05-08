#version 460 core

layout (location = 0) in vec4 ParticlePosition;
layout (location = 1) in vec4 ParticleVelocity;
layout (location = 2) in vec4 ParticleColor;

out vec4 Color;
out vec3 WorldPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

// The size of particles
uniform int ParticleSize;


void main()
{

	Color = ParticleColor;

	gl_PointSize = ParticleSize;

	WorldPosition = (WorldMatrix * vec4(ParticlePosition.xyz, 1.0)).xyz;
	
	gl_Position = ViewProjMatrix  * vec4(WorldPosition.xyz, 1.0);
}
