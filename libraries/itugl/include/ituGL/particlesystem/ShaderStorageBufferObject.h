#pragma once

#include <ituGL/core/BufferObject.h>
#include <ituGL/core/Data.h>

class ShaderStorageBufferObject : public BufferObjectBase<BufferObject::ShaderStorageBuffer>
{
public:
	ShaderStorageBufferObject();

	using BufferObject::AllocateData;

	void AllocateData(size_t size);
	void AllocateData(std::span<const std::byte> data);

	void BindBufferBase(GLuint index);
	void BindArrayBuffer();
	

	template<typename T>
	void AllocateData(std::span<const T> data, Usage usage = Usage::StaticDraw);
	template<typename T>
	inline void AllocateData(std::span<T> data, Usage usage = Usage::StaticDraw) { AllocateData(std::span<const T>(data), usage); }
	
	using BufferObject::UpdateData;
	template<typename T>
	void UpdateData(std::span<const T> data, size_t offsetBytes = 0);
	template<typename T>
	inline void Update(std::span<T> data, size_t offsetBytes = 0) { UpdateData(std::span<const T>(data), offsetBytes); }
};

// Call the base implementation with the span converted to bytes
template<typename T>
void ShaderStorageBufferObject::AllocateData(std::span<const T> data, Usage usage)
{
	AllocateData(Data::GetBytes(data), usage);
}

// Call the base implementation with the span converted to bytes
template<typename T>
void ShaderStorageBufferObject::UpdateData(std::span<const T> data, size_t offsetBytes)
{
	UpdateData(Data::GetBytes(data), offsetBytes);
}
