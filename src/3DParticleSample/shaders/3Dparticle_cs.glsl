

void main() {
	uint idx = gl_GlobalInvocationID.x;

	vec3 vPos = Positions[idx].xyz;
	vec3 vVel = Velocities[idx].xyz;
	float vBirth = Positions[idx].w; // birth time of particle
	float vMass = 1.0;
	float randomSeed = Velocities[idx].w; // mass of particle

	float deltaTime = CurrentTime - vBirth;
	float duration = Durations[idx];

	float goldenRatio = (1.0 + sqrt(5.0)) / 2.0;
	float angleIncrement = 2.0 * PI * goldenRatio;
	float angle = angleIncrement * float(idx);
	float t = (float(idx) + 0.5) / float(ParticleCount > 0 ? ParticleCount : 8192);
	float inclination = acos(1.0 - 2.0 * t);

	vec3 newPos;
	newPos.x = 3 * sin(inclination) * cos(angle);
	newPos.y = 3 * sin(inclination) * sin(angle);
	newPos.z = 3 * cos(inclination);

	vec3 initialTotalForce = (newPos - vPos) / 10;

	Update(idx, initialTotalForce, newPos);

}