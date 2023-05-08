#version 460 core

layout (location = 0) in vec4 ParticlePosition;
layout (location = 1) in vec4 ParticleVelocity;
layout (location = 2) in vec4 ParticleColor;
layout (location = 3) in vec2 aTexCoord;

out vec4 Color;
out vec3 WorldPosition;
out vec2 TexCoord;
out vec3 EyeSpacePos;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 ViewMatrix;

// The size of particles
uniform int ParticleSize;
uniform float ParticleScale;


void main()
{
	TexCoord =aTexCoord;
	Color = ParticleColor;
	EyeSpacePos = (ViewMatrix * WorldMatrix * vec4(ParticlePosition.xyz, 1.0f)).xyz;
	// gl_PointSize = ParticleSize;
	// gl_PointSize = -ParticleScale * ParticleSize / EyeSpacePos.z;
	gl_PointSize = ParticleSize;

	WorldPosition = (WorldMatrix * vec4(ParticlePosition.xyz, 1.0)).xyz;
	
	gl_Position = ViewProjMatrix  * vec4(WorldPosition.xyz, 1.0);
}
