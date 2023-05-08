#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/particlesystem/ParticleSystem.h>

class SamplerParticlesApplication : public Application
{
public:
    SamplerParticlesApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:

    void InitializeGeometry();
    void InitializeShaders();


private:


    ParticleSystem m_particleSystem;
    ShaderProgram m_renderingShaderProgram;

};
