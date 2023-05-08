#include "3DSamplerParticleApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>



ThreeDSamplerParticleApplication::ThreeDSamplerParticleApplication()
    : Application(1024, 1024, "3D Sampler")
    , m_cameraPosition(0, 0, 8)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
{
}

void ThreeDSamplerParticleApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeShaders();
    m_particleSystem.UpdateParticleCount(m_particleCount);
    m_particleSystem.Initialize(GetCurrentTime());

    // Enable GL features for particle system
    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
    m_particleSystem.BlendEnabled(device);
}

void ThreeDSamplerParticleApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();

    // Update particles when the count is changed
    if (m_originParticleCount != m_particleCount) {
        m_originParticleCount = m_particleCount;
        m_particleSystem.UpdateParticleCount(m_particleCount);
        m_particleSystem.Initialize(GetCurrentTime());
    }

}

void ThreeDSamplerParticleApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.8, 0.8, 0.8, 1.0f), true, 1.0f);

    // Render partilce 
    m_particleSystem.Render(GetCurrentTime());
    m_particleRenderingShaderProgram.Use();
    m_particleSystem.BindVAO();
    glDrawArrays(GL_POINTS, 0, m_particleSystem.GetParticleCount());

    // Set model matrix
    glm::mat4 modelMatrix = glm::mat4{1.0f};
    if (m_rotation) { // when open rotation option
        modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f*GetCurrentTime()), glm::vec3(1.0, 0.0, 0.0));
    }
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

    // Update Uniforms
    m_particleRenderingShaderProgram.SetUniform(m_particleSizeLocation, m_particleSize);
    m_particleRenderingShaderProgram.SetUniform(m_particleColorLocation, m_particleColor);
    glUniform1i(m_randomColorLocation, m_randomColor);
    m_particleRenderingShaderProgram.SetUniform(m_worldMatrixLocation, modelMatrix);
    m_particleRenderingShaderProgram.SetUniform(m_viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());


    //// Define a rotation axis and rotation speed
    //const glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    //const float rotationSpeed = 5.0f; // Adjust the speed as needed

    //// Initialize the rotation angle to 0
    //float rotationAngle = 0.0f;

    //// In your rendering loop, update the rotation angle and the rotation matrix
    //rotationAngle += rotationSpeed;
    //glm::mat4 rotationMatrix;
    ////rotationMatrix = glm::rotate(rotationMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));;
    //rotationMatrix = glm::rotate(rotationMatrix, (float)GetCurrentTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    ////m_particleRenderingShaderProgram.SetUniform(m_rotationMatrixLocation, rotationMatrix);

    //// Render the debug user interface
    RenderGUI();

}

void ThreeDSamplerParticleApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    m_particleSystem.Cleanup();

    Application::Cleanup();
}

void ThreeDSamplerParticleApplication::InitializeCamera()
{
    // Set view matrix
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f, 0.0f, 3.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void ThreeDSamplerParticleApplication::InitializeShaders()
{   
    // Register the compute shader 
    m_particleSystem.RegisterComputeShader("shaders/3Dparticle_cs.glsl");

    // Build Rendering shaders (vertex and fragment)
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load("shaders/3Dparticle.vert");
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load("shaders/3Dparticle.frag");
    m_particleRenderingShaderProgram.Build(vertexShader, fragmentShader);

    // Get Uniforms Location
    m_worldMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("WorldMatrix");
    m_viewProjMatrixLocation = m_particleRenderingShaderProgram.GetUniformLocation("ViewProjMatrix");
    m_particleSizeLocation = m_particleRenderingShaderProgram.GetUniformLocation("ParticleSize");
    m_particleColorLocation = m_particleRenderingShaderProgram.GetUniformLocation("ParticleUniformColor");
    m_randomColorLocation = m_particleRenderingShaderProgram.GetUniformLocation("RandomColor");

    // Set Default Uniforms
    m_particleRenderingShaderProgram.SetUniform(m_worldMatrixLocation, glm::scale(glm::vec3(1.0f)));
    m_particleRenderingShaderProgram.SetUniform(m_viewProjMatrixLocation, m_camera.GetViewProjectionMatrix());
}

void ThreeDSamplerParticleApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    ImGui::Text("Particle Count");
    ImGui::SameLine();
    ImGui::RadioButton("128", &m_particleCount, 128);
    ImGui::SameLine();
    ImGui::RadioButton("256", &m_particleCount, 256);
    ImGui::SameLine();
    ImGui::RadioButton("512", &m_particleCount, 512);
    ImGui::SameLine();
    ImGui::RadioButton("1024", &m_particleCount, 1024);
    ImGui::SameLine();
    ImGui::RadioButton("2048", &m_particleCount, 2048);
    ImGui::SameLine();
    ImGui::RadioButton("4096", &m_particleCount, 4096);
    ImGui::SameLine();
    ImGui::RadioButton("8192", &m_particleCount, 8192);

    ImGui::Separator();
    ImGui::ColorEdit3("Particle Color", glm::value_ptr(m_particleColor));
    ImGui::SameLine();
    ImGui::Checkbox("Random Color", &m_randomColor);
    ImGui::InputInt("Particle Size", &m_particleSize);
    ImGui::Separator();
    ImGui::Checkbox("Rotate", &m_rotation);


    m_imGui.EndFrame();
}

void ThreeDSamplerParticleApplication::UpdateCamera()
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
