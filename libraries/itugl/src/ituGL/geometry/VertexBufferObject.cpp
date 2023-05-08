#include <ituGL/geometry/VertexBufferObject.h>
#include <vector>

VertexBufferObject::VertexBufferObject()
{
    // Nothing to do here, it is done by the base class
}

// Call the base implementation with Usage::StaticDraw
void VertexBufferObject::AllocateData(size_t size)
{
    AllocateData(size, Usage::StaticDraw);
}

// Call the base implementation with Usage::StaticDraw
void VertexBufferObject::AllocateData(std::span<const std::byte> data)
{
    AllocateData(data, Usage::StaticDraw);
}

void VertexBufferObject::GetBufferDataSetting()
{
    GLint bufferSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    m_bufferData = std::vector<float>(bufferSize / sizeof(float));
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, m_bufferData.data());
}
