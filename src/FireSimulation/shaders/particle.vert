#version 460 core

layout (location = 0) in vec4 ParticlePosition;
layout (location = 1) in vec4 ParticleVelocity;
layout (location = 2) in vec4 ParticleColor;

out vec4 Color;
out vec3 WorldPosition;
out vec4 Pos;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 RotationMatrix;

uniform vec3 ParticleUniformColor;
uniform int ParticleSize;
uniform bool RandomColor;

void main()
{
	Color = ParticleColor;
	Pos = ParticlePosition;

	// Change the particle size with the life-span (hack the particle size)
	float size = ParticleColor.y;
	float Age = Color.w;
    float Life = Color.z;
    float LifeSpan = Age / Life;
	if (LifeSpan > 0.3)
		size = ParticleColor.y/10.0;
	if (LifeSpan > 0.7)
		size = ParticleColor.y/6.0;
	if (LifeSpan > 0.9)
		size = 1.0;
	gl_PointSize = size;

	WorldPosition = (WorldMatrix * vec4(ParticlePosition.xyz, 1.0)).xyz;
	gl_Position = ViewProjMatrix  * vec4(WorldPosition.xyz, 1.0);
}
