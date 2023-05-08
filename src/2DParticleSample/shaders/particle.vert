#version 460 core

layout (location = 0) in vec4 ParticlePosition;
layout (location = 1) in vec4 ParticleVelocity;
layout (location = 2) in vec4 ParticleColor;

out vec4 Color;

void main()
{
	Color = ParticleColor;

	gl_PointSize = 10;
	gl_Position = vec4(ParticlePosition.xyz, 1.0f);
}
