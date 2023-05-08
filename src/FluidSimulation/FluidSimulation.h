#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGL/texture/Texture2DObject.h>
#include <ituGL/texture/FramebufferObject.h>

#include <ituGL/particlesystem/ParticleSystem.h>

#include <ituGL/texture/TextureCubemapObject.h>

class Material;

class FluidSimulation : public Application
{
public:
    FluidSimulation();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeMaterial();
    void InitializeRenderer();
    void InitializeModels();
    void InitializeLights();
    void RenderGUI();
    void FluidInitialize();

private:
    GLuint CreateTexture(GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type);

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;

    Scene m_scene;
    // Renderer
    Renderer m_renderer;

    // Materials
    std::shared_ptr<Material> m_material;

    // Create a Particle system 
    ParticleSystem m_particleSystem;
    ShaderProgram m_particleRenderingShaderProgram;

    // Particle settings
    int m_particleCount = 128*128;
    int m_particleSize = 7;

    ShaderProgram::Location m_worldMatrixLocation;
    ShaderProgram::Location m_viewProjMatrixLocation;
    ShaderProgram::Location m_particleSizeLocation;

    // Particle Object Data
    std::vector<glm::vec4> m_particlePosData;
    std::vector<glm::vec4> m_particleVelData;
    std::vector<Color> m_particleColorData;
    std::vector<float> m_particleDurationData;



    // Water cube 
    VertexBufferObject m_cubeVbo;
    VertexArrayObject m_cubeVao;

    // variables for sph unfirom
    /*float m_smoothingLength = 0.1;
    float m_particleMass = 0.02;
    float m_baseDensity = 100;
    float m_viscosity = 0.5;
    float m_stiffness = 20;*/

    std::shared_ptr<Material> m_waterMaterial;
    std::shared_ptr<Texture2DObject> m_waterTexture;


};
