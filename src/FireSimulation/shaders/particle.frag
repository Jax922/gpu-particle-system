#version 460 core

out vec4 FragColor;

in vec4 Color;
in vec4 Pos;

uniform sampler2D FlameTexture;

// Flame Colors setting
const vec3 white =  vec3(1.0f,1.0f,1.0f);
const vec3 yellow = vec3(1.0f,0.8f,0.0f);
const vec3 orange = vec3(1.0f,0.4f,0.0f);
const vec3 red =    vec3(1.0f,0.1f,0.0f);

// The Flame color is more white when the partciles more closer to center
float whiteAreaSize(float distance, float maxSize) {
    return 1.0 - smoothstep(0.0, maxSize, distance);
}

void main()
{
    
    vec2 pointCoords = gl_PointCoord * 2 - 1;
    float len = length(pointCoords);
    if (len > 1) {
		discard;
	}

    // Calculate the particle life-span
    float Age = Color.w;
    float Life = Color.z;
    float LifeSpan = Age / Life;

    // Flame texture and alpha
    vec4 texColor = texture(FlameTexture, gl_PointCoord);
    float alpha = texColor.r * (1.0 - LifeSpan);
  
    // Calculate the distance of current particle position from the center  
    float distanceFromCenter = length(Pos.xy - vec2(0.0, 0.0));

    // The Flame color is more white when the partciles more closer to center
    float whiteFactor = whiteAreaSize(distanceFromCenter, 6.0); 

    // Mix all flame colors
    vec3 color = mix(yellow, orange, LifeSpan);
    color = mix(color, red, LifeSpan * LifeSpan);
    color = mix(color, white, whiteFactor*whiteFactor);

    FragColor = vec4(color, alpha) * texColor;
   
}
