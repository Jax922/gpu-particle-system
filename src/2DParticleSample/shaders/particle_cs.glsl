
void emitParticle(uint idx, float randomSeed) {
	Positions[idx] = vec4(RandomRange(randomSeed, -0.5, 0.5), 0.5, 0, CurrentTime);
	Velocities[idx] = vec4(0, -0.1, 0, Velocities[idx].w);
}

// air resistance coffience
const float resistanceK = 0.7;

void main() {
	uint idx = gl_GlobalInvocationID.x;

	if (idx >= Positions.length()) {
		return;
	}

	vec3 vPos = Positions[idx].xyz;
	vec3 vVel = Velocities[idx].xyz;
	float vBirth = Positions[idx].w; // birth time of particle
	float randomSeed = Velocities[idx].w; // mass of particle

	float deltaTime = CurrentTime - vBirth;
	float duration = Durations[idx];

	if (deltaTime >= duration) { // not alive
		emitParticle(idx, randomSeed);
	}
	else {
		// based on the X = 1/2 * g * t^2 , but also concern the air resistance
		vPos += 0.5f * (1-resistanceK)* Gravity * 0.1 * 0.1;
		Positions[idx].xyz = vPos;
	}

    

}