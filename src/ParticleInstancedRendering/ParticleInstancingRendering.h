#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>

#include <ituGL/particlesystem/ParticleSystem.h>
#include <ituGL/texture/TextureCubemapObject.h>

class Material;

class ParticleInstancingRendering : public Application
{
public:
    ParticleInstancingRendering();

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
    
    void RenderingModelWithParticles(const int& index);

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

    // Create a Particle System 
    ParticleSystem m_particleSystem;
    ShaderProgram m_particleRenderingShaderProgram;

    // Particle setting
    int m_particleCount = 128;
    int m_particleSize = 5;

    // Particle Object Data
    std::vector<glm::vec4> m_particlePosData;
    std::vector<glm::vec4> m_particleVelData;
    std::vector<Color> m_particleColorData;
    std::vector<float> m_particleDurationData;

    // Uniform Location
    ShaderProgram::Location m_worldMatrixLocation;
    ShaderProgram::Location m_viewProjMatrixLocation;
    ShaderProgram::Location m_particleSizeLocation;

    bool m_showParticle = false;

    // Model options for Particle rendering test
    int m_show_model_index = 0;
    int m_show_model_previous = 0;

    // models: Bunny, Armadillo, and dragon
    std::vector<const char*> m_model_path = {
        "models/bunny.obj",
        "models/armadillo.obj",
        "models/dragon.obj"
    };

    // get vertex data of the model
    std::vector<std::vector<std::vector<float>>> m_vertexDatas;
    std::vector<int> m_strides;

};
