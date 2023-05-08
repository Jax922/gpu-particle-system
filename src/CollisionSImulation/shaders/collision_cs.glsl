
// The elasticity of particle , which is from 0 to 1
uniform float ParticleElasticity;


void emitParticle(uint idx, float randomSeed) {
    // Init Position of particle
	Positions[idx] = vec4(RandomRange(randomSeed, -2.0, 2.5), RandomRange(randomSeed, 1.0, 6.0), 0, CurrentTime);
    // Simulate the mass of particle
    float mass = RandomRange(randomSeed, 0.2, 8.0);
    // Init Velocity of particle
	Velocities[idx] = vec4(0, -0.05, 0, mass);
    // Init duration of particle
    Durations[idx] = 1000000000;
}

void main() {
	uint idx = gl_GlobalInvocationID.x;

	if (idx >= Positions.length()) {
		return;
	}
	
	vec3 vPos = Positions[idx].xyz; // Position
	vec3 vVel = Velocities[idx].xyz; // Velocity
	float vBirth = Positions[idx].w; // birth time of particle
	float randomSeed = Velocities[idx].w; // helper for emit particle

	float deltaTime = CurrentTime - vBirth;
	float duration = Durations[idx];

    // Compute the force of gravity
    vec3 forceGravity = 1 * Gravity;

    // Simulate the resistance force when the particle fall in the are
    float forceResistanceK = 0.05;

    // Calculate the final force of the particle
    vec3 force = forceGravity - (forceResistanceK * vVel);
 
    // a = F/M
    vec3 a = force/1;
    // x = x0 + v0 * t
    vPos = vPos + vVel * 0.05;
    // v = v0 + a*t
    vVel = vVel + a * 0.05;

    if(deltaTime >= duration) {
        emitParticle(idx, randomSeed);
    } else {
        // Calculate the distance of the partcile and the ground
        float distance = vPos.y - 0;

        // Handle collision 
        if (distance <= 0.01) {
            vPos.y = 0;
            vVel = -1*vVel*ParticleElasticity;
            Velocities[idx].xyz = vVel;
            if(abs(vVel.y) <= 1.0) {
                Velocities[idx].xyz = vec3(0,0,0);
            }
        } else {
            Velocities[idx].xyz = vVel;
            Positions[idx].xyz = vPos;
        }
    }
}