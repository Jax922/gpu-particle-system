#include <ituGL/particlesystem/ShaderStorageBufferObject.h>

ShaderStorageBufferObject::ShaderStorageBufferObject()
{
}

void ShaderStorageBufferObject::AllocateData(size_t size)
{
	AllocateData(size, Usage::StaticDraw);
}

void ShaderStorageBufferObject::AllocateData(std::span<const std::byte> data)
{
	AllocateData(data, Usage::StaticDraw);
}

void ShaderStorageBufferObject::BindBufferBase(GLuint index)
{
	Handle handle = GetHandle();
	glBindBufferBase(BufferObject::ShaderStorageBuffer, index, handle);
}

void ShaderStorageBufferObject::BindArrayBuffer()
{
	Handle handle = GetHandle();
	glBindBuffer(BufferObject::ArrayBuffer, handle);
}


