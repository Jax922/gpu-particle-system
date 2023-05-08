#version 460 core

out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in vec3 WorldPosition;
in vec3 EyeSpacePos;

//in vec3 LightDir;

uniform mat4 InvProjMatrix;
uniform mat4 ProjMatrix;
uniform mat4 InvViewMatrix;
uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;

uniform int ParticleSize;

uniform sampler2D DepthTex;
uniform sampler2D ThickTex;


// hack for the camera psotion
const vec3 CameraPosition = vec3(0.0f, 5.0f, 8.0f);

struct DirLight
{
	vec3 direction;
	float specular;
	float diffuse;
};

const float specularK = 10.0;
const float diffuseK = 0.4;

vec3 uvToEye(vec2 coord, float z)
{
	vec2 pos = coord * 2.0f - 1.0f;
	vec4 clipPos = vec4(pos, z, 1.0f);
	vec4 viewPos = InvProjMatrix * clipPos;

	return viewPos.xyz / viewPos.w;
}

uniform sampler2D WaterTexture;

void main()
{

	// Sample the texture to get the color of the particle
	vec4 textureColor = texture(WaterTexture, gl_PointCoord);

	// Use a light blue color for the particles
	vec4 particleColorDiff = vec4(0.5, 0.5, 1.0, 1.0);

	// Blend the particle color with the texture color using the alpha value of the texture color
	vec4 finalColor = Color * (1.0 - textureColor.a) + textureColor * textureColor.a;
	FragColor = vec4(finalColor.rgb, 0.1);

	vec3 normal;
	normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0,1.0);
	float mag = dot(normal.xy, normal.xy);
	if(mag > 1.0) discard;

	// normal.z = sqrt(1.0 - mag);
	// // fragColor = vec4(normal.z*0.005, 0.0, 0.0, 1.0);

	// //CalculateNormal();
    // FragColor = Color;

	// vec3 ambient = 0.2 * FragColor.xyz;
    // // diffuse
    // vec3 lightDir = normalize(vec3(1, -1, 1)); // hack for light direction
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * 0.6 * FragColor.xyz;

	// FragColor.xyz = ambient + diffuse;
	// // gamma correction.
	// const float gamma = 2.2f;
	// FragColor.rgb = pow(FragColor.rgb,vec3(1.0f/gamma));

	// // glow map.

	// vec4 pixelEyePos = vec4(EyeSpacePos + vec3(normal.x, normal.y, -normal.z) * ParticleSize, 1.0f);
	// vec4 pixelClipPos = ProjMatrix * pixelEyePos;
	// gl_FragDepth = pixelClipPos.z / pixelClipPos.w;
}
