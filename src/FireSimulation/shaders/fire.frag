#version 460 core

out vec4 FragColor;

in vec4 Color;

//in vec3 LightDir;

// hack for the camera psotion
const vec3 CameraPosition = vec3(0.0f, 5.0f, 8.0f);

void main()
{

	vec2 pointCoords = gl_PointCoord * 2 - 1;
	float len = length(pointCoords);
    vec3 rgb = vec3(1, 1, 1);
    vec3 lightDir = normalize(vec3(1, -1, 1)); // hack for light direction
    float x = pointCoords.x;
    float y = pointCoords.y;
    float pho = x * x + y * y;
    float z = sqrt(1 - pho);
    if (pho > 1) discard;
    vec4 rgba = vec4(dot(lightDir, vec3(x, y, z)) * Color.rgb, 1);
    vec4 white = vec4(dot(lightDir, vec3(x, y, z)) * vec3(1, 1, 1), 1) + 0.2;
    
    FragColor = rgba;
}
