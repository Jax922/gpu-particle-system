// constant settings
#define pi 3.1415927410125732421875f
const float SmoothingLength = 0.1;
const float Mass = 0.02;
const float BaseDensity = 80;
const float Viscosity = 0.5;
const float Stiffness = 20;

// uniform float SmoothingLength;
// uniform float Mass;
// uniform float BaseDensity;
// uniform float Viscosity;
// uniform float Stiffness;


const float Poly6Kernel = 315.0 / (64.0 * pi * pow(SmoothingLength, 9.0));
const float SpikyKernel = -(45.0*Mass) / (pi * pow(SmoothingLength, 6.0));
const float ViscosityKernel = (45.0*Viscosity*Mass) / (pi * pow(SmoothingLength, 6.0));
const float Epsilon = 1e-6;

// Boundary setting
const vec3 boundaryMin = vec3(-0.5, -0.5, 0.0); // Lower boundary values (x, y, z)
const vec3 boundaryMax = vec3(0.5, 0.5, 0.5); // Upper boundary values (x, y, z)

 // Density computation
float calculateDensity(uint idx ) {
    float density = 0.0;
    vec3 pos = Positions[idx].xyz;
    for (uint i = 0; i < Positions.length(); i++) {
        vec3 neighborPos = Positions[i].xyz;
        float distance = length(pos - neighborPos);
        float radiusSquared = SmoothingLength * SmoothingLength - distance * distance;

        if (distance <= SmoothingLength) {
            density += Mass * Poly6Kernel * radiusSquared * radiusSquared * radiusSquared;
        }
    }
    Velocities[idx].w = density;
    return density;
}

// Pressure computation
float calculatePressure(uint idx) {
    float density = Velocities[idx].w;
    float pressure = max(0.0, Stiffness*(density - BaseDensity));
    Colors[idx].w = pressure;
    return pressure;
}

// Force compuation
vec3 calculateForce(uint idx) {
    vec3 pressureForce = vec3(0.0);
    vec3 viscosityForce = vec3(0.0);

    vec3 pos = Positions[idx].xyz;
    for (uint i = 0; i < Positions.length(); i++) {
        if (i == idx) continue;

        vec3 neighborPos = Positions[i].xyz;
        float distance = length(pos - neighborPos);
        vec3 direction = normalize(pos - neighborPos);

        if (distance <= SmoothingLength) {
            float neighborDensity = max(1.0, Velocities[i].w); // Velocities.w store density values
            float neighborPressure = max(0.0, neighborDensity - BaseDensity);
            vec3 neighborVelocity = Velocities[i].xyz;

            pressureForce -= Mass * (Colors[idx].w+Colors[i].w) / (2.0f * Velocities[i].w) * -45.f / (pi * pow(SmoothingLength, 6)) * pow(SmoothingLength - distance, 2) * direction;

            viscosityForce += Mass * (Velocities[i].xyz - Velocities[idx].xyz) / Velocities[i].w * 45.f / (pi * pow(SmoothingLength, 6)) * (SmoothingLength - distance);

        }
    }
    viscosityForce *= 30.0;
    vec3 externalForce = Velocities[idx].w * vec3(0, -98.0665, 0);

    return pressureForce + viscosityForce + externalForce;
}

// Update the positon and velocity
void sph(uint idx, float dt) {
    float density = calculateDensity(idx);
    float pressure = calculatePressure(idx);
    vec3 force = calculateForce(idx);


    vec3 newVel = Velocities[idx].xyz +(force / (Velocities[idx].w+Epsilon)) * dt;
    vec3 newPos = Positions[idx].xyz + newVel * dt;
    
    float damping = 0.5;

    for (int i = 0; i < 3; i++) {
        if (newPos[i] <= boundaryMin[i]) {
            newPos[i] = boundaryMin[i];
            newVel[i] = -newVel[i] * damping;
        }
        else if (newPos[i] >= boundaryMax[i]) {
            newPos[i] = boundaryMax[i];
            newVel[i] = -newVel[i] * damping;
        }
    }
    
    // Update particle position and velocity
    Positions[idx].xyz = newPos;
    Velocities[idx].xyz = newVel;

}