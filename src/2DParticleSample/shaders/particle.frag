#version 460 core

out vec4 FragColor;

in vec4 Color;

void main()
{
	vec2 pointCoords = gl_PointCoord * 2 - 1;
	float len = length(pointCoords);
    if (len > 1) {
		discard;
	} else {
		FragColor = vec4(Color.rgb, 1.0);
	}
}
