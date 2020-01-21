#include "framebuffer.hpp"

#include <iostream>
#include <stdexcept>

#include "texture.hpp"


namespace internal {
	std::shared_ptr<gl::Texture> createAndCheckColorTexture(int width, int height, std::shared_ptr<gl::Texture> texture) {
		if (texture == nullptr) {
			// Create a new texture
			texture = std::make_shared<gl::Texture>(height, width);
			texture->magFilterType = GL_NEAREST;
			texture->minFilterType = GL_NEAREST;
			texture->generateMipMap = false;
		}

		// Check texture attributes
		assert(texture->magFilterType == GL_NEAREST && texture->minFilterType == GL_NEAREST);
		assert(texture->textureType == GL_TEXTURE_2D);

		if (texture->cols != width || texture->rows != height) {
			std::cout << "WARNING: Attached texture with a different size than framebuffer. Going to resize texture" << std::endl;
			texture->resize(width, height);
		}
		
		if (texture->generateMipMap) {
			std::cout << "WARNING: Used a texture which indicates mip map generation for a render target" << std::endl;
		}

		return texture;
	}

	std::shared_ptr<gl::Texture> createAndCheckDepthTexture(int width, int height, std::shared_ptr<gl::Texture> texture, bool depthAndStencil) {
		GLenum internalFormat = depthAndStencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT;
		GLenum format = depthAndStencil ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT;
		GLenum type = depthAndStencil ? GL_UNSIGNED_INT_24_8 : GL_FLOAT;
		
		if (texture == nullptr) {
			// Create a new texture
			texture = std::make_shared<gl::Texture>(height, width, 
				format, 
				internalFormat, 
				type);
			texture->magFilterType = GL_NEAREST;
			texture->minFilterType = GL_NEAREST;
			texture->generateMipMap = false;
			
		}

		// Check texture attributes
		assert(texture->magFilterType == GL_NEAREST && texture->minFilterType == GL_NEAREST);
		assert(texture->textureType == GL_TEXTURE_2D);

		if (texture->cols != width || texture->rows != height) {
			std::cout << "WARNING: Attached texture with a different size than framebuffer. Going to resize texture" << std::endl;
			texture->resize(width, height);
		}

		if (texture->generateMipMap) {
			std::cout << "WARNING: Used a texture which indicates mip map generation for a render target" << std::endl;
		}

		// Check data types
		if (texture->internalFormat != internalFormat || texture->exposedFormat != format || texture->dataType != type) {
			std::cout << "WARNING: Unexpected internal format" << std::endl;
		}

		return texture;
	}

	GLenum getRenderBufferStorageDataType(GLenum attachment) {
		switch (attachment) {
		case GL_DEPTH_STENCIL_ATTACHMENT:
			return GL_DEPTH24_STENCIL8;
		case GL_DEPTH_ATTACHMENT:
			return GL_DEPTH_COMPONENT;
		default:
			return GL_RGBA;
		}
	}
}

gl::Framebuffer* gl::Framebuffer::s_boundBuffer = NULL;

std::shared_ptr<gl::Texture> gl::Framebuffer::setRenderTexture(int attachment, std::shared_ptr<gl::Texture> texture)
{
	assert(attachment < mColorAttachments.size());
	assert(attachment >= 0 && attachment < GL_MAX_COLOR_ATTACHMENTS);

	texture = internal::createAndCheckColorTexture(mWidth, mHeight, texture);

	mColorAttachments[attachment] = std::move(FramebufferAttachment(GL_COLOR_ATTACHMENT0 + attachment, texture));

	mRequriesUpdate = true;

	return texture;
}

std::shared_ptr<gl::Texture> gl::Framebuffer::appendRenderTexture(std::shared_ptr<gl::Texture> texture)
{
	assert(mColorAttachments.size() < GL_MAX_COLOR_ATTACHMENTS - 1);
	
	texture = internal::createAndCheckColorTexture(mWidth, mHeight, texture);

	mColorAttachments.emplace_back(FramebufferAttachment(GL_COLOR_ATTACHMENT0 + mColorAttachments.size(), texture));

	mRequriesUpdate = true;

	return texture;
}

void gl::Framebuffer::appendColorBuffer()
{
	assert(mColorAttachments.size() < GL_MAX_COLOR_ATTACHMENTS - 1);

	mColorAttachments.emplace_back(FramebufferAttachment(GL_COLOR_ATTACHMENT0 + mColorAttachments.size()));

	mRequriesUpdate = true;
}

void gl::Framebuffer::clear(std::initializer_list<glm::vec4> clearColors)
{
	gl::Framebuffer* bound_buffer = s_boundBuffer;
	if (this != s_boundBuffer) {
		bind();
	}
	// clear depth and stencils
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	int idx = 0;
	for (glm::vec4 col : clearColors) {
		if (idx >= mColorAttachments.size()) {
			std::cerr << "WARNING: got more clear colors than attachments" << std::endl;
			break;
		}
		glClearBufferfv(GL_COLOR, idx++, &col[0]);
	}
	if(this != bound_buffer)
		bound_buffer->bind();
}

void gl::Framebuffer::clearColorAttachment(int slot, glm::vec4 clearColor)
{
	gl::Framebuffer* bound_buffer = s_boundBuffer;
	if (this != s_boundBuffer) {
		bind();
	}
	glClearBufferfv(GL_COLOR, slot, &clearColor[0]);
	if (this != bound_buffer)
		bound_buffer->bind();
}

gl::Framebuffer::Framebuffer(int width, int height) :
	mId(0),
	mWidth(width),
	mHeight(height),
	mDepthAttachment(GL_DEPTH_ATTACHMENT),
	mRequriesUpdate(true)
{
	mColorAttachments.emplace_back(FramebufferAttachment(GL_COLOR_ATTACHMENT0));
}

gl::Framebuffer::~Framebuffer()
{
	if (mId != 0) {
		glDeleteFramebuffers(1, &mId);
		if (s_boundBuffer == this) {
			s_boundBuffer = NULL;
		}
		mId = 0;
	}
}

std::shared_ptr<gl::Texture> gl::Framebuffer::setDepthTexture(std::shared_ptr<gl::Texture> texture, bool depthAndStencil)
{
	texture = internal::createAndCheckDepthTexture(mWidth, mHeight, texture, depthAndStencil);
	const GLenum attachment = depthAndStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
	mDepthAttachment = FramebufferAttachment(attachment, texture);
	mRequriesUpdate = true;
	return texture;
}

void gl::Framebuffer::update()
{
	if (!mRequriesUpdate)
		return;

	// Destroy the fbo and the attached depth buffer
	if (mId != 0) {
		glDeleteFramebuffers(1, &mId);
	}

	// Recreate fbo
	glGenFramebuffers(1, &mId);
	
	mDepthAttachment.attach(mId, mWidth, mHeight);
	
	// Resize all color textures
	std::vector<GLenum> drawBuffers;
	for (int i = 0; i < (int)mColorAttachments.size(); ++i) {
		mColorAttachments[i].attach(mId, mWidth, mHeight);
		drawBuffers.push_back(mColorAttachments[i].attachment);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mId);
	glDrawBuffers(drawBuffers.size(), drawBuffers.data());

	GLenum framebufferState = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferState != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error creating framebuffer" << std::endl;
		throw std::runtime_error("Error creating framebuffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mRequriesUpdate = false;
}

void gl::Framebuffer::resize(int width, int height)
{
	if (height == mHeight && width == mWidth)
		return;

	mHeight = height;
	mWidth = width;
	mRequriesUpdate = true;	
}

void gl::Framebuffer::blitToDefaultBuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	bind();
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(
		0, 0, mWidth, mHeight,
		0, 0, mWidth, mHeight,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		GL_NEAREST);
	unbind();

}

glm::uvec4 gl::Framebuffer::readColorPixel(int col, int row, int slot)
{
	assert(slot < mColorAttachments.size());
	bind();
	std::vector<unsigned char> buffer(4);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + slot);
	glReadPixels(col, mHeight - row, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	return glm::vec4(buffer[0], buffer[1], buffer[2], buffer[3]);
}

std::shared_ptr<gl::Texture> gl::Framebuffer::getRenderTexture(int slot)
{
	return mColorAttachments[slot].targetTexture;
}

void gl::Framebuffer::bind()
{
	if (mRequriesUpdate)
		update();
	glBindFramebuffer(GL_FRAMEBUFFER, mId);
	s_boundBuffer = this;
}

void gl::Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	s_boundBuffer = NULL;
}

gl::Framebuffer::FramebufferAttachment::FramebufferAttachment() :
	targetBuffer(0),
	targetTexture(nullptr),
	attachment(0)
{
}

gl::Framebuffer::FramebufferAttachment::FramebufferAttachment(GLenum attachment, std::shared_ptr<gl::Texture> texture) :
	targetTexture(texture),
	targetBuffer(0),
	attachment(attachment)
{
	// Make sure that the texture is actually created
	if(targetTexture != nullptr) 
		targetTexture->update();
}

gl::Framebuffer::FramebufferAttachment::FramebufferAttachment(FramebufferAttachment&& other)
{
	if (&other != this) {
		if (targetBuffer != 0) {
			glDeleteRenderbuffers(1, &targetBuffer);
		}
		targetBuffer = other.targetBuffer;
		targetTexture = other.targetTexture;
		attachment = other.attachment;

		other.targetTexture = nullptr;
		other.targetBuffer = 0;
	}
}

gl::Framebuffer::FramebufferAttachment::~FramebufferAttachment()
{
	if (targetBuffer != 0) {
		glDeleteRenderbuffers(1, &targetBuffer);
	}
}

gl::Framebuffer::FramebufferAttachment& gl::Framebuffer::FramebufferAttachment::operator=(gl::Framebuffer::FramebufferAttachment&& other)
{
	if (this == &other) {
		return *this;
	}
	if (targetBuffer != 0) {
		glDeleteRenderbuffers(1, &targetBuffer);
	}

	targetBuffer = other.targetBuffer;
	targetTexture = other.targetTexture;
	attachment = other.attachment;

	other.targetBuffer = 0;
	other.targetTexture = nullptr;
}

void gl::Framebuffer::FramebufferAttachment::attach(GLuint framebuffer, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	if (targetTexture != nullptr) {
		targetTexture->resize(width, height);
		// Ensure that resize actually is updated
		targetTexture->update();
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, targetTexture->id, 0);
	}
	else {
		if (targetBuffer != 0) {
			glDeleteRenderbuffers(1, &targetBuffer);
		}
		glGenRenderbuffers(1, &targetBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, targetBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, internal::getRenderBufferStorageDataType(attachment), width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, targetBuffer);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
