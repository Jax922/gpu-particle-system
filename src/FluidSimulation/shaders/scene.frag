//Inputs
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
//Outputs
out vec4 FragColor;

//Uniforms
uniform vec3 Color;
uniform vec3 CameraPosition;
uniform sampler2D ColorTexture;

const float AmbientReflection = 1.0f;
const float DiffuseReflection = 1.0f;
const float SpecularReflection = 1.0f;
const float SpecularExponent = 30.0f;

const vec3 AmbientColor = vec3(0.1, 0.1, 0.1);
const vec3 LightColor = vec3(0.3, 0.3, 0.3);
const vec3 LightPosition = vec3(-10.0f, 20.0f, 10.0f);


//  ====================== Blinn-Phong Lighting ============================================

vec3 GetAmbientReflection(vec3 objectColor)
{
	return AmbientColor * AmbientReflection * objectColor;
}

vec3 GetDiffuseReflection(vec3 objectColor, vec3 lightVector, vec3 normalVector)
{
	return LightColor * DiffuseReflection * objectColor * max(dot(lightVector, normalVector), 0.0f);
}

vec3 GetSpecularReflection(vec3 lightVector, vec3 viewVector, vec3 normalVector)
{
	vec3 halfVector = normalize(lightVector + viewVector);
	return LightColor * SpecularReflection * pow(max(dot(halfVector, normalVector), 0.0f), SpecularExponent);
}

vec3 GetBlinnPhongReflection(vec3 objectColor, vec3 lightVector, vec3 viewVector, vec3 normalVector)
{
	return GetAmbientReflection(objectColor)
		 + GetDiffuseReflection(objectColor, lightVector, normalVector)
		 + GetSpecularReflection(lightVector, viewVector, normalVector);
}
//  ====================== Blinn-Phong Lighting ============================================

void main()
{
	vec4 objectColor = vec4(Color * texture(ColorTexture, TexCoord).rgb, 1.0);
	vec3 lightVector = normalize(LightPosition - WorldPosition);
	vec3 viewVector = normalize(CameraPosition - WorldPosition);
	vec3 normalVector = normalize(WorldNormal);

	// Set the Blinn-Phong Lighting
	FragColor = vec4(GetBlinnPhongReflection(objectColor.rgb, lightVector, viewVector, normalVector), 1.0f);
}