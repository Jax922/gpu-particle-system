#include "FluidSimulation.h"

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
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>



#include <ituGL/renderer/ForwardRenderPass.h>
#include <ituGL/renderer/SkyboxRenderPass.h>
#include <iostream>

#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>

#include <glm/gtx/string_cast.hpp>



FluidSimulation::FluidSimulation()
    : Application(1024, 1024, "Collision Simulation")
    , m_renderer(GetDevice())
{
}

void FluidSimulation::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeLights();
    InitializeMaterial();
    InitializeModels();
    InitializeRenderer();

    // Fliud data init
    FluidInitialize();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
    m_particleSystem.BlendEnabled(device);

}

void FluidSimulation::Update()
{
    Application::Update();

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());

    // Set renderer 
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);

}

void FluidSimulation::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.8f, 0.8f, 0.8f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();


    // Render particles
    m_particleSystem.Render(GetCurrentTime());

    const Camera& camera = *m_cameraController.GetCamera()->GetCamera();
  
    // Particle rendering
    {
        m_waterMaterial->Use();
        ShaderProgram& shaderProgram = *(m_waterMaterial->GetShaderProgram());

        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("WorldMatrix"), glm::mat4(1.0f));
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ViewProjMatrix"), camera.GetViewProjectionMatrix());
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ViewMatrix"), camera.GetViewMatrix());
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("InvViewMatrix"), glm::inverse(camera.GetViewMatrix()));
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ProjMatrix"), camera.GetProjectionMatrix());
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("InvProjMatrix"), glm::inverse(camera.GetProjectionMatrix()));
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ParticleSize"), m_particleSize);
        // Get the main window's aspect ratio
        float aspectRatio = GetMainWindow().GetAspectRatio();
        // Set the field of view angle in degrees
        float fieldOfViewDegrees = 30.0f;
        // Calculate the ParticleScale value based on the aspect ratio and field of view
        float particleScale = 1.0f * 1024 / aspectRatio * (1.0f / tanf(glm::radians(fieldOfViewDegrees) * 0.5f));
        shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ParticleScale"), particleScale);

        m_particleSystem.BindVAO();
        glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());
        glUseProgram(0);
    }

    // Render the debug user interface
    RenderGUI();
}

void FluidSimulation::Cleanup()
{
    // Cleanup 
    m_imGui.Cleanup();
    m_particleSystem.Cleanup();
    Application::Cleanup();
}

void FluidSimulation::InitializeCamera()
{
    // Create the camera
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->SetViewMatrix(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0), glm::vec3(0.0f, 1.0f, 0.0));
    camera->SetPerspectiveProjectionMatrix(1.0f, GetMainWindow().GetAspectRatio(), 0.1f, 100.0f);
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", camera);
    m_scene.AddSceneNode(sceneCamera);

    m_cameraController.SetCamera(sceneCamera);
}

void FluidSimulation::InitializeMaterial()
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
    std::vector<const char*> computeShaderPaths = {
        "shaders/sph.glsl",
        "shaders/main_cs.glsl"
    };
    m_particleSystem.RegisterComputeShader(computeShaderPaths);

    // Build particles rendering shader(vertex and fragment)
    Shader particleVertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/fluid.vert");
    Shader particleFragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/fluid.frag");
    //m_particleRenderingShaderProgram.Build(particleVertexShader, particleFragmentShader);
    std::shared_ptr<ShaderProgram> particleRenderingShaderProgram = std::make_shared<ShaderProgram>();
    particleRenderingShaderProgram->Build(particleVertexShader, particleFragmentShader);
    m_waterMaterial = std::make_shared<Material>(particleRenderingShaderProgram);
    Texture2DLoader textureLoader(TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    textureLoader.SetFlipVertical(true);
    m_waterTexture = textureLoader.LoadShared("textures/water.png");
    m_waterMaterial->SetUniformValue("WaterTexture", m_waterTexture);
}

void FluidSimulation::InitializeRenderer()
{
    m_renderer.AddRenderPass(std::make_unique<ForwardRenderPass>());
}

void FluidSimulation::InitializeModels()
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
    Texture2DLoader textureLoader(TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    textureLoader.SetFlipVertical(true);
    groundModel->GetMaterial(0).SetUniformValue("ColorTexture", textureLoader.LoadShared("textures/floor.png"));
    //groundModel->GetMaterial(0).SetUniformValue("ColorTexture", )


    m_scene.AddSceneNode(std::make_shared<SceneModel>("ground", groundModel));

}

void FluidSimulation::InitializeLights()
{
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLight->SetDirection(glm::vec3(-0.3f, -1.0f, -0.3f)); 
    directionalLight->SetIntensity(3.0f);
    m_scene.AddSceneNode(std::make_shared<SceneLight>("directional light", directionalLight));
}

void FluidSimulation::FluidInitialize()
{
    // Init particle data
    for (size_t i = 0; i < m_particleCount; i++)
    {
        m_particlePosData.push_back(glm::vec4(
        m_particleSystem.RandomRange(-0.4f, 0.0f), m_particleSystem.RandomRange(0.0f, 0.5f), m_particleSystem.RandomRange(0.0f, 0.4f), 0));
        m_particleVelData.push_back(glm::vec4(0, -0.1, 0, 0));
        m_particleColorData.push_back(glm::vec4(.275f, 0.65f, 0.85f, 0.5f));
        m_particleDurationData.push_back(1000000000.0f);
    }

    // Update particle count
    m_particleSystem.UpdateParticleCount(m_particleCount);

    // Update particle data
    m_particleSystem.UpdatePositionData(m_particlePosData);
    m_particleSystem.UpdateVelocityData(m_particleVelData);
    m_particleSystem.UpdateColorData(m_particleColorData);
    m_particleSystem.UpdateDurationData(m_particleDurationData);

    // Init particle system
    m_particleSystem.Initialize(GetCurrentTime(), false);
}

void FluidSimulation::RenderGUI()
{
    m_imGui.BeginFrame();

    if (auto window = m_imGui.UseWindow("Particle Rendering"))
    {
        // Show the FPS in GUI 
        ImGui::Text("FPS= %f", ImGui::GetIO().Framerate);

        // SPH settings
        /*if (ImGui::TreeNodeEx("SPH Seettings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Smoothing Length", &m_smoothingLength);
            ImGui::InputFloat("Particle Mass", &m_particleMass);
            ImGui::InputFloat("Basic Density", &m_baseDensity);
            ImGui::InputFloat("Viscosity", &m_viscosity);
            ImGui::InputFloat("Stiffness", &m_stiffness);
            ImGui::TreePop();
        }*/

        // Rendering setting
        if (ImGui::TreeNodeEx("Rendering Seettings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("Particle Size", &m_particleSize);
            ImGui::TreePop();
        }
    }

    m_imGui.EndFrame();
}