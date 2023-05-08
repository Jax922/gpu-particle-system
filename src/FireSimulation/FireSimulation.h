#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/particlesystem/ParticleSystem.h>

#include <ituGL/camera/CameraController.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGL/application/Window.h>
#include <ituGL/shader/Material.h>
#include <ituGL/texture/Texture2DObject.h>

class FireSimulation : public Application
{
public:
    FireSimulation();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:

    void InitializeGeometry();
    void InitializeShaders();
    void InitializeCamera();

    void UpdateCamera();

    void RenderGUI();

private:


    ParticleSystem m_particleSystem;
    ShaderProgram m_particleRenderingShaderProgram;

    // Particle Object Data
    std::vector<glm::vec4> m_particlePosData;
    std::vector<glm::vec4> m_particleVelData;
    std::vector<Color> m_particleColorData;
    std::vector<float> m_particleDurationData;

    // Particle settings
    int m_particleCount = 128 * 512;
    int m_particleSize = 5;

    // fire settings
    glm::vec3 m_fireCenter;
    float m_fireRadius;

    // Uniform Locations
    ShaderProgram::Location m_worldMatrixLocation;
    ShaderProgram::Location m_viewProjMatrixLocation;
    ShaderProgram::Location m_modelMatrixLocation;
    ShaderProgram::Location m_particleSizeLocation;
    ShaderProgram::Location m_particleColorLocation;
    ShaderProgram::Location m_randomColorLocation;

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

    // Camera controller
    CameraController m_cameraController;

    std::shared_ptr<Material> m_material;
    std::shared_ptr<Texture2DObject> m_particleTexture;
    std::shared_ptr<Texture2DObject> m_flameTexture;

};
