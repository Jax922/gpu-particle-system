// Define some constant variables
const vec3 Gravity = vec3(0, -9.8f, 0);
const float PI = 3.1415926;

// Generate a random value according the range
float RandomRange(float seed, float from, float to) {
	return seed * (to - from) + from;
}

// Compute the distance of two points
float PointDistance(vec3 p1, vec3 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

// Compute the signed distance of two points
float SingedPointDitance(vec3 p1, vec3 p2, vec3 direction) {
    vec3 dis = p2 - p1;
    return dot(dis, direction);
}