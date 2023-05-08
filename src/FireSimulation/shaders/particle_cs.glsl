
void emitParticle(uint idx) {
	Positions[idx].x = Colors[idx].x;
	Positions[idx].y = 0;
	Positions[idx].w = CurrentTime;
	Velocities[idx].w = 1;
	Colors[idx].w = 0;
}


void main() {
	uint idx = gl_GlobalInvocationID.x;

	if (idx >= Positions.length()) {
		return;
	}

	vec3 vPos = Positions[idx].xyz;
	vec3 vVel = Velocities[idx].xyz;
	float vBirth = Positions[idx].w; // birth time of particle
	float deltaTime = CurrentTime - vBirth;
	float duration = Durations[idx];

	// Now, the colors data is store the deltatime(age) and duration(life)
	// Those data will be use in fragment shader to calculate the particle color and particle size
	Colors[idx].w = deltaTime;
	Colors[idx].z = duration;

	if (deltaTime >= duration) { // not alive
		emitParticle(idx);
	}
	else {
		// Update the partilce postion 
		if(Velocities[idx].w == 1){
			vPos += vVel * 0.08;
			Positions[idx].xyz = vPos;
		}
	}

    

}