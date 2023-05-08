#include <ituGL/texture/FramebufferObject.h>

#include <ituGL/texture/Texture2DObject.h>
#include <cassert>

std::shared_ptr<const FramebufferObject> FramebufferObject::s_defaultFramebuffer(std::make_shared<FramebufferObject>(FramebufferObject(Object::NullHandle)));

FramebufferObject::FramebufferObject() : Object(NullHandle)
{
    Handle& handle = GetHandle();
    glGenFramebuffers(1, &handle);
}

FramebufferObject::FramebufferObject(Handle handle) : Object(handle)
{
    // Currently only allowed for private default object
    assert(handle == NullHandle);
}

FramebufferObject::~FramebufferObject()
{
    Handle& handle = GetHandle();
    if (handle != NullHandle)
    {
        glDeleteFramebuffers(1, &handle);
    }
}

void FramebufferObject::Bind() const
{
    Bind(Target::Both);
}

void FramebufferObject::Bind(Target target) const
{
    Handle handle = GetHandle();
    glBindFramebuffer(static_cast<GLenum>(target), handle);
}

void FramebufferObject::Unbind()
{
    Unbind(Target::Both);
}

void FramebufferObject::Unbind(Target target)
{
    Handle handle = NullHandle;
    glBindFramebuffer(static_cast<GLenum>(target), handle);
}

std::shared_ptr<const FramebufferObject> FramebufferObject::GetDefault()
{
    return FramebufferObject::s_defaultFramebuffer;
}

void FramebufferObject::SetTexture(Target target, Attachment attachment, const Texture2DObject& texture, int level)
{
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment), texture.GetTarget(), texture.GetHandle(), level);
}

void FramebufferObject::SetDrawBuffers(std::span<const Attachment> attachments)
{
    glDrawBuffers(static_cast<GLint>(attachments.size()), reinterpret_cast<const GLenum*>(attachments.data()));
}
