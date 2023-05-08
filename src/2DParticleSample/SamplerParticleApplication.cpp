#include "SamplerParticleApplication.h"

#include <ituGL/shader/Shader.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <cassert>
#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <ituGL/asset/ShaderLoader.h>

SamplerParticlesApplication::SamplerParticlesApplication()
    : Application(1024, 1024, "Particles demo")
{
}

void SamplerParticlesApplication::Initialize()
{
    InitializeGeometry();

    InitializeShaders();

    // Enable some GL features
    GetDevice().EnableFeature(GL_PROGRAM_POINT_SIZE);
    // Enable GL_BLEND to have blending on the particles, and configure it as additive blending
    GetDevice().EnableFeature(GL_BLEND);
    GetDevice().SetVSyncEnabled(true);

    m_particleSystem.Initialize(GetCurrentTime());
}

void SamplerParticlesApplication::Update()
{
    Application::Update();
}

void SamplerParticlesApplication::Render()
{
    // Clear background
    GetDevice().Clear(Color(0.7f, 0.7f, 0.7f));

    // Render particles
    m_particleSystem.Render(GetCurrentTime());
    m_renderingShaderProgram.Use();
    m_particleSystem.BindVAO();
    glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());

    Application::Render();
}

void SamplerParticlesApplication::Cleanup()
{
    m_particleSystem.Cleanup();
    Application::Cleanup();
}

void SamplerParticlesApplication::InitializeGeometry()
{
}

void SamplerParticlesApplication::InitializeShaders()
{
    // register compute shader
    m_particleSystem.RegisterComputeShader("shaders/particle_cs.glsl");

    // build rendering shader (vertex and fragment)
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/particle.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/particle.frag");
    m_renderingShaderProgram.Build(vertexShader, fragmentShader);
   
}