#include "FireSimulation.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ituGL/asset/Texture2DLoader.h>



FireSimulation::FireSimulation()
    : Application(1024, 1024, "3D Sampler")
    , m_cameraPosition(0, 4, 20)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
    , m_fireCenter(glm::vec3(0,0,0))
    , m_fireRadius(0.5f)
{
}

void FireSimulation::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeShaders();
    InitializeGeometry();

    // Enable GL features for particle system
    DeviceGL& device = GetDevice();
    m_particleSystem.BlendEnabled(device);
    glDisable(GL_DEPTH_TEST);


}

void FireSimulation::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();

}

void FireSimulation::Render()
{
    Application::Render();
    
    // Clear color and depth
    GetDevice().Clear(true, Color(0.0, 0.0, 0.0, 1.0f), true, 1.0f);

    // Render partilce 
    m_particleSystem.Render(GetCurrentTime());
    m_material->Use();
    ShaderProgram& shaderProgram = *(m_material->GetShaderProgram());

    // Set model matrix
    glm::mat4 modelMatrix = glm::mat4{ 1.0f };
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

    // Update Uniforms
    shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ParticleSize"), m_particleSize);
    shaderProgram.SetUniform(shaderProgram.GetUniformLocation("WorldMatrix"), modelMatrix);
    shaderProgram.SetUniform(shaderProgram.GetUniformLocation("ViewProjMatrix"), m_camera.GetViewProjectionMatrix());

    m_particleSystem.BindVAO();
    glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());

    // Render the debug user interface
    RenderGUI();
}

void FireSimulation::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    m_particleSystem.Cleanup();

    Application::Cleanup();
}

void FireSimulation::InitializeCamera()
{
    // Set view matrix
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f, 0.0f, 3.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void FireSimulation::InitializeShaders()
{
    // register compute shader
    m_particleSystem.RegisterComputeShader("shaders/particle_cs.glsl");

    // build rendering shader (vertex and fragment)
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/particle.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/particle.frag");
    std::shared_ptr<ShaderProgram> particleRenderingShaderProgram = std::make_shared<ShaderProgram>();
    particleRenderingShaderProgram->Build(vertexShader, fragmentShader);
    m_material = std::make_shared<Material>(particleRenderingShaderProgram);

    Texture2DLoader textureLoader(TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA8);
    textureLoader.SetFlipVertical(true);

    //m_particleTexture = textureLoader.LoadShared("textures/particle.bmp");
    m_flameTexture = textureLoader.LoadShared("textures/fire.png");
    
    //m_material->SetUniformValue("ParticleTexture", m_particleTexture);
    m_material->SetUniformValue("FlameTexture", m_flameTexture);

}


void FireSimulation::RenderGUI()
{
    m_imGui.BeginFrame();

  
    /*ImGui::InputInt("Particle Size", &m_particleSize);
    ImGui::Separator();*/


    m_imGui.EndFrame();
}

void FireSimulation::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
    {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0, 1, 0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

    // Update view matrix
    m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}


void FireSimulation::InitializeGeometry()
{

    std::srand(time(0));
    // Init particle data
    for (size_t i = 0; i < m_particleCount; i++)
    {   
        glm::vec3 pos(0.0f);

        // Init the partilces postion in x and z aixs
        for (size_t x = 0; x < 10; x++)
        {
            pos.x += (2.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f);
            pos.z += (2.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 1.0f);
        }

        m_particlePosData.push_back(glm::vec4(pos.x*m_fireRadius, 0, pos.z * m_fireRadius, 0));
        float particleSize = m_particleSystem.RandomRange(1, 8);

        float velX = 0.0;
        float origin = 0.0;
        float posX = pos.x * m_fireRadius;
        float posZ = pos.z * m_fireRadius;
        float center = posX * posX + posZ * posZ;

        // Calculate the velocity in x axis
        if (center < 0.5)
        {
            velX = m_particleSystem.RandomRange(-0.1, 0.1);
            origin = 1.0;
        }
     
        // push particle data
        m_particleVelData.push_back(glm::vec4(velX, m_particleSystem.RandomRange(0.1, 0.8), 0, 0));
        m_particleColorData.push_back(glm::vec4(pos.x*m_fireRadius, particleSize, 0.05f, 0.5f));
        m_particleDurationData.push_back(m_particleSystem.RandomRange(1.0, 2.0));
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
