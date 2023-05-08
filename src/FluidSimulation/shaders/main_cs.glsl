


void main() {
	uint idx = gl_GlobalInvocationID.x;

	if (idx >= Positions.length()) {
		return;
	}

    sph(idx, 0.001);

}