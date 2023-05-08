#version 460 core

layout (location = 0) in vec4 ParticlePosition;
layout (location = 1) in vec4 ParticleVelocity;
layout (location = 2) in vec4 ParticleColor;

out vec4 Color;
out vec4 Vel;
out vec3 WorldPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 RotationMatrix;

uniform vec3 ParticleUniformColor;
uniform int ParticleSize;
uniform bool RandomColor;

void main()
{
	// Color = ParticleColor;
	if (RandomColor) {
		Color = ParticleColor;
	} else {
		Color = vec4(ParticleUniformColor, 1.0);
	}

	Vel = ParticleVelocity;
	//float age = CurrentTime - ParticleBirth;

	gl_PointSize =ParticleSize;

	WorldPosition = (WorldMatrix * vec4(ParticlePosition.xyz, 1.0)).xyz;
	
	gl_Position = ViewProjMatrix  * vec4(WorldPosition.xyz, 1.0);
}
