// Euler method for ODE
// reference : https://en.m.wikipedia.org/wiki/Euler_method

vec3 EulerIntergral(vec3 x, vec3 y, float deltaTime)
{
    return y + deltaTime * x;
}
