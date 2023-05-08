#include "CollisionSimulation.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/shader/Material.h>
#include <ituGL/renderer/PostFXRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>
#include <imgui.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/lighting/PointLight.h>
#include <ituGL/scene/SceneLight.h>

#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>


#include <ituGL/renderer/ForwardRenderPass.h>
#include <ituGL/renderer/SkyboxRenderPass.h>
#include <iostream>

CollisionSimulation::CollisionSimulation()
    : Application(1024, 1024, "Collision Simulation")
    , m_renderer(GetDevice())
{
}

void CollisionSimulation::Initialize()
{
    Application::Initialize();

    InitializeCamera();
    InitializeLights();
    InitializeMaterial();
    InitializeModels();
    InitializeRenderer();

    m_particleSystem.UpdateParticleCount(m_particleCount);
    m_particleSystem.Initialize(GetCurrentTime());

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
    m_particleSystem.BlendEnabled(device);

}

void CollisionSimulation::Update()
{
    Application::Update();

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    // Set renderer 
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);

}

void CollisionSimulation::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.8f, 0.8f, 0.8f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render particles
    m_particleSystem.Render(GetCurrentTime());
    m_particleSystem.UpdateUniform("ParticleElasticity", m_particleElasticity);
    const Camera& camera = *m_cameraController.GetCamera()->GetCamera();

    m_particleRenderingShaderProgram.Use();
    m_particleSystem.BindVAO();
    glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());

    // Update Uniform
    m_particleRenderingShaderProgram.SetUniform(m_worldMatrixLocation, glm::mat4(1.0f));
    m_particleRenderingShaderProgram.SetUniform(m_viewProjMatrixLocation, camera.GetViewProjectionMatrix());
    m_particleRenderingShaderProgram.SetUniform(m_particleSizeLocation, m_particleSize);

    // Render the debug user interface
    //RenderGUI();
}

void CollisionSimulation::Cleanup()
{
    // Cleanup 
    m_particleSystem.Cleanup();
    Application::Cleanup();
}

void CollisionSimulation::InitializeCamera()
{
    // Create the camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(0.0f, 2.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0), glm::vec3(0.0f, 1.0f, 0.0));
    camera->SetPerspectiveProjectionMatrix(1.0f, GetMainWindow().GetAspectRatio(), 0.1f, 100.0f);
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);
    m_scene.AddSceneNode(sceneCamera);

    m_cameraController.SetCamera(sceneCamera);
}

void CollisionSimulation::InitializeMaterial()
{
    // Load and build scene shader
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version460.glsl");
    vertexShaderPaths.push_back("shaders/scene.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version460.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back("shaders/scene.frag");

    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    shaderProgramPtr->Build(vertexShader, fragmentShader);

    // Get transform related uniform locations
    ShaderProgram::Location cameraPositionLocation = shaderProgramPtr->GetUniformLocation("CameraPosition");
    ShaderProgram::Location worldMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("ViewProjMatrix");

    // Register shader with renderer
    m_renderer.RegisterShaderProgram(shaderProgramPtr,
        [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            if (cameraChanged)
            {
                shaderProgram.SetUniform(cameraPositionLocation, camera.ExtractTranslation());
                shaderProgram.SetUniform(viewProjMatrixLocation, camera.GetViewProjectionMatrix());
            }
            glm::vec3 translation(0.0f, -2.0f, 0.0f);
            glm::mat4 newWorldMatrix = glm::translate(worldMatrix, translation);
            shaderProgram.SetUniform(worldMatrixLocation, newWorldMatrix);
        },
        m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
    );

    // Filter out uniforms that are not material properties
    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("CameraPosition");
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");
    filteredUniforms.insert("LightIndirect");
    filteredUniforms.insert("LightColor");
    filteredUniforms.insert("LightPosition");
    filteredUniforms.insert("LightDirection");
    filteredUniforms.insert("LightAttenuation");

    // Create reference material
    assert(shaderProgramPtr);
    m_material = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);


    // Register Computer Shader for particle
    m_particleSystem.RegisterComputeShader("shaders/collision_cs.glsl");

    // Build particles rendering shader(vertex and fragment)
    Shader particleVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/collision.vert");
    Shader particleFragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/collision.frag");
    m_particleRenderingShaderProgram.Build(particleVertexShader, particleFragmentShader);

    // Set Unfiroms for particle rendering shader
    m_worldMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("WorldMatrix");
    m_viewProjMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("ViewProjMatrix");
    m_particleSizeLocation = m_particleRenderingShaderProgram.GetUniformLocation("ParticleSize");

}

void CollisionSimulation::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<ForwardRenderPass>());
}

void CollisionSimulation::InitializeModels()
{

    m_material->SetUniformValue("AmbientColor", glm::vec3(0.25f));
    m_material->SetUniformValue("Color", glm::vec3(1.0f));

    ModelLoader loader(m_material);
    loader.GetTexture2DLoader().SetFlipVertical(true);

    // Link vertex properties to attributes
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Tangent, "VertexTangent");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Bitangent, "VertexBitangent");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

    // Link material properties to uniforms
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseColor, "Color");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseTexture, "ColorTexture");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::NormalTexture, "NormalTexture");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::SpecularTexture, "SpecularTexture");

    // Load ground model
    std::shared_ptr<Model> groundModel = loader.LoadShared("models/ground/ground.obj");
    m_scene.AddSceneNode(std::make_shared<SceneModel>("ground", groundModel));

}

void CollisionSimulation::InitializeLights()
{
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLight->SetDirection(glm::vec3(-0.3f, -1.0f, -0.3f)); 
    directionalLight->SetIntensity(3.0f);
    m_scene.AddSceneNode(std::make_shared<SceneLight>("directional light", directionalLight));
}
