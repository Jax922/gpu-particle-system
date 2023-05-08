#include <ituGL/particlesystem/ParticleSystem.h>

#include <ituGL/asset/ShaderLoader.h>
#include <iostream>

#include <glm/gtx/string_cast.hpp>


ParticleSystem::ParticleSystem() 
    : m_groupSize(128)
    , m_particleCount(128*64)
{
}


ParticleSystem::ParticleSystem(const unsigned int groupSize, const unsigned int particleCount) 
    : m_groupSize(groupSize)
    , m_particleCount(particleCount)
{
}

void ParticleSystem::Initialize(float currentTime)
{
    InitializeData(currentTime, true);
    InitializeGeometry();
}

void ParticleSystem::Initialize(float currentTime, bool initData)
{
    InitializeData(currentTime, initData);
    InitializeGeometry();
}

void ParticleSystem::Cleanup()
{
    m_ssbo_pos.Unbind();
    m_ssbo_vel.Unbind();
    m_ssbo_col.Unbind();
    m_ssbo_duration.Unbind();
    m_vao.Unbind();
}

void ParticleSystem::Render(float currentTime) {
    m_computeShaderProgram.Use();
    m_computeShaderProgram.SetUniform(m_currentTimeUniform, currentTime);
    glUniform1i(m_particleCountLocation, m_particleCount);
    glDispatchCompute(m_particleCount/128, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::InitializeData(float& currentTime, bool initData)
{
    // initilaize basic particles data , if the initData = true
    if (initData) {
        for (size_t i = 0; i < m_particleCount; i++)
        {
            m_particlePosData.push_back(glm::vec4(0, 0, 0, 0));
            m_particleVelData.push_back(glm::vec4(0, 0, 0, Random01()));
            m_particleColorData.push_back(RandomColor());
            m_particleDurationData.push_back(RandomRange(0.0, 5.0));
        }
    }

    // For debug
  /*  for (const auto& element : m_particlePosData) {
        std::cout << glm::to_string(element) << std::endl;
    }*/

    // Create Position SSBO buffer 
    m_ssbo_pos.Bind();
    m_ssbo_pos.AllocateData(std::span(m_particlePosData), BufferObject::Usage::DynamicDraw);
    m_ssbo_pos.BindBufferBase(0);
    // Create Velocity SSBO buffer 
    m_ssbo_vel.Bind();
    m_ssbo_vel.AllocateData(std::span(m_particleVelData), BufferObject::Usage::DynamicDraw);
    m_ssbo_vel.BindBufferBase(1);
    // Create Color SSBO buffer 
    m_ssbo_col.Bind();
    m_ssbo_col.AllocateData(std::span(m_particleColorData), BufferObject::Usage::DynamicDraw);
    m_ssbo_col.BindBufferBase(2);
    // Create Duration SSBO buffer 
    m_ssbo_duration.Bind();
    m_ssbo_duration.AllocateData(std::span(m_particleDurationData), BufferObject::Usage::DynamicDraw);
    m_ssbo_duration.BindBufferBase(3);

    // Get Uniform Location
    m_currentTimeUniform = m_computeShaderProgram.GetUniformLocation("CurrentTime");
    m_particleCountLocation = m_computeShaderProgram.GetUniformLocation("ParticleCount");
}

void ParticleSystem::InitializeGeometry()
{
    m_vao.Bind();
    GLsizei stride = sizeof(glm::vec4);
    GLint offset = 0;
    GLuint location = 0;

    // Set the Position data
    m_ssbo_pos.BindArrayBuffer();
    m_vao.SetAttribute(0, VertexAttribute(Data::Type::Float, 4), offset, stride);
    offset += VertexAttribute(Data::Type::Float, 4).GetSize();
    // Set the Velocity data
    m_ssbo_vel.BindArrayBuffer();
    m_vao.SetAttribute(1, VertexAttribute(Data::Type::Float, 4), offset, stride);
    offset += VertexAttribute(Data::Type::Float, 4).GetSize();
    // Set the Color data
    m_ssbo_col.BindArrayBuffer();
    m_vao.SetAttribute(2, VertexAttribute(Data::Type::Float, 4), offset, stride);

    // Unbind SSBO and VAO
    ShaderStorageBufferObject::Unbind();
    VertexArrayObject::Unbind();
}

void ParticleSystem::RegisterComputeShader(std::vector<const char*> paths)
{
    std::string shaderFilePath = std::string(__FILE__);
    shaderFilePath = shaderFilePath.substr(0, shaderFilePath.find_last_of("\\/"));
    // Import default shaders for Compute Shader
    std::string versionPath = shaderFilePath + "/shaders/version460.glsl";
    std::string utilsPath = shaderFilePath+"/shaders/particle_system/utils.glsl";
    std::string intergalSolverPtah = shaderFilePath+"/shaders/particle_system/intergral_solver.glsl";
    std::string updateParticleDataPath = shaderFilePath+"/shaders/particle_system/update_particle_data.glsl";
    paths.insert(paths.begin(), {versionPath.c_str(), utilsPath.c_str(), intergalSolverPtah.c_str(), updateParticleDataPath.c_str()});
    Shader computeShader = ShaderLoader(Shader::ComputeShader).Load(paths);
    m_computeShaderProgram.Build(computeShader);
}

void ParticleSystem::RegisterComputeShader(const char* path)
{
    std::string shaderFilePath = std::string(__FILE__);
    shaderFilePath = shaderFilePath.substr(0, shaderFilePath.find_last_of("\\/"));
    // Import default shaders for Compute Shader
    std::string versionPath = shaderFilePath + "/shaders/version460.glsl";
    std::string utilsPath = shaderFilePath + "/shaders/particle_system/utils.glsl";
    std::string intergalSolverPtah = shaderFilePath + "/shaders/particle_system/intergral_solver.glsl";
    std::string updateParticleDataPath = shaderFilePath + "/shaders/particle_system/update_particle_data.glsl";
    std::vector<const char*> paths;
    paths.insert(paths.begin(), { versionPath.c_str(), utilsPath.c_str(), intergalSolverPtah.c_str(), updateParticleDataPath.c_str(), path});
    Shader computeShader = ShaderLoader(Shader::ComputeShader).Load(paths);
    m_computeShaderProgram.Build(computeShader);
}

void ParticleSystem::BlendEnabled(DeviceGL& device)
{
    // Enable some GL features for particle system
    device.EnableFeature(GL_PROGRAM_POINT_SIZE);
    device.EnableFeature(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    device.SetVSyncEnabled(true);
}

// Supoort Update uniform feature 
void ParticleSystem::UpdateUniform(const char* name, float value)
{
    auto location = m_computeShaderProgram.GetUniformLocation(name);
    m_computeShaderProgram.SetUniform(location, value);
}

void ParticleSystem::UpdateUniform(const char* name, int value)
{
    auto location = m_computeShaderProgram.GetUniformLocation(name);
    m_computeShaderProgram.SetUniform(location, value);
}

void ParticleSystem::UpdateUniform(const char* name, glm::vec3 value)
{
    auto location = m_computeShaderProgram.GetUniformLocation(name);
    m_computeShaderProgram.SetUniform(location, value);
}

void ParticleSystem::UpdateUniform(const char* name, glm::vec4 value)
{
    auto location = m_computeShaderProgram.GetUniformLocation(name);
    m_computeShaderProgram.SetUniform(location, value);
}

void ParticleSystem::UpdateUniform(const char* name, glm::mat4 value)
{
    auto location = m_computeShaderProgram.GetUniformLocation(name);
    m_computeShaderProgram.SetUniform(location, value);
}

































float ParticleSystem::Random01()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

float ParticleSystem::RandomRange(float from, float to)
{
    return Random01() * (to - from) + from;
}

glm::vec2 ParticleSystem::RandomDirection()
{
    return glm::normalize(glm::vec2(Random01() - 0.5f, Random01() - 0.5f));
}

Color ParticleSystem::RandomColor()
{
    return Color(Random01(), Random01(), Random01());
}
