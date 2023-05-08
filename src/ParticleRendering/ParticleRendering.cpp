#include "ParticleRendering.h"

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
#include <thread>

// Init the color of particles
std::vector<glm::vec4> PARTICLE_COLORS = {
    glm::vec4(0.0, 0.0, 0.25, 1.0),
    glm::vec4(0.53, 0.81, 0.92, 1.0),
    glm::vec4(0.94, 0.5, 0.5, 1.0),
    glm::vec4(0.13, 0.54, 0.13, 1.0),
    glm::vec4(0.85, 0.65, 0.13, 1.0),
    glm::vec4(0.85, 0.44, 0.84, 1.0),
    glm::vec4(0.98, 0.5, 0.45, 1.0),
    glm::vec4(0.25, 0.88, 0.82, 1.0)
};

ParticleRendering::ParticleRendering()
    : Application(1024, 1024, "Particle Rendering")
    , m_renderer(GetDevice())
{
}

void ParticleRendering::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeLights();
    InitializeMaterial();
    InitializeModels();
    InitializeRenderer();
    
    // Setting of the Particle System
    RenderingModelWithParticles(0);
    m_particleSystem.Initialize(GetCurrentTime());

    // Enable Gl features for Particle System
    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
    m_particleSystem.BlendEnabled(device);
}

void ParticleRendering::Update()
{
    Application::Update();

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor); 

    // Update the particle data when the model is changed
    if (m_show_model_previous != m_show_model_index) {
        m_show_model_previous = m_show_model_index;
        RenderingModelWithParticles(m_show_model_index);
        m_particleSystem.Initialize(GetCurrentTime());
    }

}

void ParticleRendering::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render particle system
    m_particleSystem.Render(GetCurrentTime());
    const Camera& camera = *m_cameraController.GetCamera()->GetCamera();

    // Render the particles
    m_particleRenderingShaderProgram.Use();
    m_particleSystem.BindVAO();
    glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());

    // Update Uniforms
    m_particleRenderingShaderProgram.SetUniform(m_worldMatrixLocation, glm::mat4(1.0f));
    m_particleRenderingShaderProgram.SetUniform(m_viewProjMatrixLocation, camera.GetViewProjectionMatrix());
    m_particleRenderingShaderProgram.SetUniform(m_particleSizeLocation, m_particleSize);

    // Render the debug user interface
    RenderGUI();
}

void ParticleRendering::Cleanup()
{
    m_imGui.Cleanup();
    m_particleSystem.Cleanup();
    Application::Cleanup();
}

void ParticleRendering::InitializeCamera()
{
    // Create the main camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(-2.0f, 2.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0), glm::vec3(0.0f, 1.0f, 0.0));
    camera->SetPerspectiveProjectionMatrix(1.0f, GetMainWindow().GetAspectRatio(), 0.1f, 100.0f);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);

    // Add the camera node to the scene
    m_scene.AddSceneNode(sceneCamera);

    // Set the camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}

void ParticleRendering::InitializeMaterial()
{
    // Load and build shader
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version460.glsl");
    vertexShaderPaths.push_back("shaders/default.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version460.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back("shaders/default.frag");

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

    // Register the Computer Shader for Particle System
    m_particleSystem.RegisterComputeShader("shaders/particle_rendering_cs.glsl");
    
    // Build particles rendering shader(vertex and fragment)
    Shader particleVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/particle_rendering.vert");
    Shader particleFragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/particle_rendering.frag");
    m_particleRenderingShaderProgram.Build(particleVertexShader, particleFragmentShader);

    // Uniform settings
    m_worldMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("WorldMatrix");
    m_viewProjMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("ViewProjMatrix");
    m_particleSizeLocation = m_particleRenderingShaderProgram.GetUniformLocation("ParticleSize");

}

void ParticleRendering::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<ForwardRenderPass>());
}

void ParticleRendering::InitializeModels()
{

    // Uniform settings
    m_material->SetUniformValue("AmbientColor", glm::vec3(0.25f));
    m_material->SetUniformValue("Color", glm::vec3(1.0f));

    std::cout << "Models loading start" << std::endl;
    // Load models
    for (auto path : m_model_path) {
        ModelLoader loader;
        loader.LoadShared(path);
        m_vertexDatas.push_back(loader.GetBufferDatas());
        m_strides.push_back(ModelLoader::TotalStride);

    }

    ModelLoader loader(m_material);

    // Flip vertically textures loaded by the model loader
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

    std::cout << "Models loading done " << std::endl;

}

void ParticleRendering::InitializeLights()
{
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLight->SetDirection(glm::vec3(-0.3f, -1.0f, -0.3f)); 
    directionalLight->SetIntensity(3.0f);
    m_scene.AddSceneNode(std::make_shared<SceneLight>("directional light", directionalLight));
}

void ParticleRendering::RenderGUI()
{
    m_imGui.BeginFrame();

    if (auto window = m_imGui.UseWindow("Particle Rendering"))
    {
        // Show the FPS in GUI 
        ImGui::Text("FPS= %f", ImGui::GetIO().Framerate);

        if (ImGui::TreeNodeEx("Model Options", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::RadioButton("Stanford Bunny", &m_show_model_index, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Stanford Armadillo", &m_show_model_index, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Dragon", &m_show_model_index, 2);
            ImGui::TreePop();
        }
         if (ImGui::TreeNodeEx("Rendering Seettings", ImGuiTreeNodeFlags_DefaultOpen))
         {
             ImGui::InputInt("Particle Size", &m_particleSize);
             ImGui::TreePop();
         }
    }

    m_imGui.EndFrame();
}

// Render a 3D model by the particles
void ParticleRendering::RenderingModelWithParticles(const int& index)
{

    m_particlePosData.clear();
    m_particleVelData.clear();
    m_particleColorData.clear();
    m_particleDurationData.clear();
   int dataSize = (int)m_vertexDatas[index][0].size();
   m_particleSystem.UpdateParticleCount(dataSize/m_strides[index]);
   for (size_t i = 0; i <dataSize; i=i+m_strides[index])
   {    
       m_particlePosData.push_back(glm::vec4(m_vertexDatas[index][0][i + 0], m_vertexDatas[index][0][i + 1], m_vertexDatas[index][0][i + 2], 0));
       m_particleVelData.push_back(glm::vec4(0,0,0,0));
       m_particleColorData.push_back(PARTICLE_COLORS[(int)m_particleSystem.RandomRange(0, PARTICLE_COLORS.size()-1)]);
       m_particleDurationData.push_back(m_particleSystem.RandomRange(6.0, 10.0));
   }

   m_particleSystem.UpdatePositionData(m_particlePosData);
   m_particleSystem.UpdateVelocityData(m_particleVelData);
   m_particleSystem.UpdateColorData(m_particleColorData);
   m_particleSystem.UpdateDurationData(m_particleDurationData);
}
