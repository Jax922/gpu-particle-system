
// Positon data
layout(std430, binding = 0) buffer Pos {
	vec4 Positions[];
};

// Velocity data
layout(std430, binding = 1) buffer Vel {
	vec4 Velocities[];
};

// Color data
layout(std430, binding = 2) buffer Col {
	vec4 Colors[];
};

// Duration data
layout(std430, binding = 3) buffer Duration {
	float Durations[];
};

layout(local_size_x = 128) in;

// Uniforms
uniform float CurrentTime;
uniform int ParticleCount;

// Emit particles
void EmitParticle(uint idx) {
	Positions[idx] = vec4(0, 0, 0, CurrentTime);
	Velocities[idx] = Velocities[idx];
	Colors[idx] = Colors[idx];
}

vec3 UpdateParticleVelocity(vec3 force0, vec3 vel0, float deltaTime, float mass) {
    // Update Velocity  by Euler Explicit
    return EulerIntergral(force0/mass, vel0, deltaTime);
}

vec3 UpdateParticlePosition(vec3 vel1, vec3 pos0, float deltaTime) {
    // Update Velocity  by Euler Implicit
    return EulerIntergral(vel1, pos0, deltaTime);
}

// Init force of particle, contains Gravity
vec3 InitialForce(float mass, vec3 otherForce) {
    vec3 gravityForce = mass * Gravity;
    return gravityForce + otherForce;
}

// Update Position and Velocity data of the particles
void Update(uint idx, vec3 initialTotalForce, vec3 newPos) {

	if (idx >= Positions.length()) {
		return;
	}

	vec3 vPos = Positions[idx].xyz;
	vec3 vVel = Velocities[idx].xyz;
	float vBirth = Positions[idx].w; // birth time of particle
	float vMass = 1.0;
	float randomSeed = Velocities[idx].w; // mass of particle

	float deltaTime = CurrentTime - vBirth;
	float duration = Durations[idx];

    vec3 updatedVel = UpdateParticleVelocity(initialTotalForce, vVel, deltaTime/10, vMass);
    vec3 updatedPos = UpdateParticlePosition(updatedVel, vPos, deltaTime/10);

	
	if (SingedPointDitance(updatedPos, newPos, newPos-vec3(0, 0, 0)) <= 0.1) { // alive
		Velocities[idx].xyz = vec3(0);
   		Positions[idx].xyz = newPos;
	} else {
		Velocities[idx].xyz = updatedVel;
   		Positions[idx].xyz = updatedPos;
	}

}