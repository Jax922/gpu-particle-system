#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>

#include <ituGL/particlesystem/ParticleSystem.h>
#include <ituGL/application/Window.h>

class Texture2DObject;

class ThreeDSamplerParticleApplication : public Application
{
public:
    ThreeDSamplerParticleApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeShaders();

    void UpdateCamera();

    void RenderGUI();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPosition;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;

    // Particle System
    ParticleSystem m_particleSystem;

    // Render particle shader program (vertex and fragment)
    ShaderProgram m_particleRenderingShaderProgram;

    // Uniform Locations
    ShaderProgram::Location m_worldMatrixLocation;
    ShaderProgram::Location m_viewProjMatrixLocation;
    ShaderProgram::Location m_modelMatrixLocation;
    ShaderProgram::Location m_particleSizeLocation;
    ShaderProgram::Location m_particleColorLocation;
    ShaderProgram::Location m_randomColorLocation;

    // Variables for IMGUI 
    bool m_rotation = false;
    int m_particleCount = 128 * 64;
    int m_originParticleCount = 128 * 64;
    int m_particleSize = 5;
    glm::vec3 m_particleColor = glm::vec3{ 60, 179, 113 };
    bool m_randomColor = false;
};
