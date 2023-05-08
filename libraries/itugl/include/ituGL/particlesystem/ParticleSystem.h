#pragma once

#include <string>
#include <vector>

#include "ShaderStorageBufferObject.h"
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/core/Color.h>
#include <ituGL/core/DeviceGL.h>

class ParticleSystem {
public:
	ParticleSystem();
	ParticleSystem(const unsigned int groupSize, const unsigned int particleCount);

	// Init SSBO buffer data
	void Initialize(float currentTime);
	void Initialize(float currentTime, bool initData);

	void Render(float currentTime);
	void Cleanup();

	// Support adding compute shader files for specific task, such as, coliision simulation 
	void RegisterComputeShader(const char* path);
	void RegisterComputeShader(std::vector<const char*> paths);

	// Get or Update the partcile count 
	unsigned int inline GetParticleCount() { return m_particleCount; }
	void inline UpdateParticleCount(const int& particleCount) { m_particleCount = particleCount; }

	// Bind the VAO 
	void inline BindVAO() { m_vao.Bind(); }
	
	// Enabled some GL features for particle system
	void BlendEnabled(DeviceGL& device);

	// Support updating uniforms, that also supports add new uniforms
	void UpdateUniform(const char* name, float value);
	void UpdateUniform(const char* name, int value);
	void UpdateUniform(const char* name, glm::vec3 value);
	void UpdateUniform(const char* name, glm::vec4 value);
	void UpdateUniform(const char* name, glm::mat4 value);

	// Update the Particle data
	void inline UpdatePositionData(std::vector<glm::vec4> data) { m_particlePosData = data; }
	void inline UpdateVelocityData(std::vector<glm::vec4> data) { m_particleVelData = data; }
	void inline UpdateColorData(std::vector<Color> data) { m_particleColorData = data; }
	void inline UpdateDurationData(std::vector<float> data) { m_particleDurationData = data; }

	// Helper methods for random values
	static float Random01();
	static float RandomRange(float from, float to);
	static glm::vec2 RandomDirection();
	static Color RandomColor();


private:
	// Shader Storage Buffer Object (SSBO) used for the Compute Shader
	ShaderStorageBufferObject m_ssbo_pos;
	ShaderStorageBufferObject m_ssbo_vel;
	ShaderStorageBufferObject m_ssbo_col;
	ShaderStorageBufferObject m_ssbo_duration;

	// VAO used for rendering particle
	VertexArrayObject m_vao;

	// Particles shader program
	ShaderProgram m_computeShaderProgram;

	// Default Unfiroms for particle system
	ShaderProgram::Location m_currentTimeUniform;        
	ShaderProgram::Location m_particleCountLocation;

	// Particles Count
	unsigned int m_particleCount;
	// Current Time
	float m_currentTime;
	// Group size of the glDispatchCompute
	unsigned int m_groupSize;

	// Particle Object Data
	std::vector<glm::vec4> m_particlePosData;
	std::vector<glm::vec4> m_particleVelData;
	std::vector<Color> m_particleColorData;
	std::vector<float> m_particleDurationData;

private:
	void InitializeData(float& currentTime, bool initData);
	void InitializeGeometry();

};