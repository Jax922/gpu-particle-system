#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>

#include <ituGL/particlesystem/ParticleSystem.h>

#include <ituGL/texture/TextureCubemapObject.h>

class Material;

class CollisionSimulation : public Application
{
public:
    CollisionSimulation();

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
    void WaterCubeRendering();
    void FluidInitialize();

private:

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
    int m_particleCount = 128;
    int m_particleSize = 15;
    float m_particleElasticity = 0.5; // The elasticity of particle , which is from 0 to 1

    ShaderProgram::Location m_worldMatrixLocation;
    ShaderProgram::Location m_viewProjMatrixLocation;
    ShaderProgram::Location m_particleSizeLocation;

};
