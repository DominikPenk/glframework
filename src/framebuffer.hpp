#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <unordered_map>


namespace gl {
	class Texture;

	class Framebuffer {

	public:
		Framebuffer(int width = 0, int height = 0);
		~Framebuffer();

		/// <summary>Set a render texture for the given attachment slot</summary>
		/// <param name="slot">Color attachment slot to bind the texture to.</param>
		/// <param name="texture">Texture to use as a buffer. If this is a nullptr a new texture will be created</param>
		/// <returns>A shared pointer to the buffer texture</returns>
		std::shared_ptr<gl::Texture> setRenderTexture(int slot, std::shared_ptr<gl::Texture> texture = nullptr);

		/// <summary>Create a new color attachment slot and binds a render texture to it</summary>
		/// <param name="texture">Texture to use as a buffer.  If this is a nullptr a new texture will be created</param>
		/// <returns>A shared pointer to the buffer texture.</returns>
		std::shared_ptr<gl::Texture> appendRenderTexture(std::shared_ptr<gl::Texture> texture = nullptr);

		/// <summary>Create a new color attachment slot and binds a renderbuffer to it.</summary>
		void appendColorBuffer();

		void clear(std::initializer_list<glm::vec4> clearColors);

		void clearColorAttachment(int slot, glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));

		/// <summary>Sets a texture as target for the depth component</summary>
		/// <param name="texture">Texture to write to. If this is a nullptr a new texture will be created</param>
		/// <param name="depthAndStencil">If set to true the texture will capture stencil and depth component simultaniously</param>
		/// <returns>A shared pointer to the buffer texture.</returns>
		std::shared_ptr<gl::Texture> setDepthTexture(std::shared_ptr<gl::Texture> texture = nullptr, bool depthAndStencil = false);

		void update();

		/// <summary>Resizes the Framebuffer (meaning all its render buffers and textures)</summary>
		void resize(int width, int height);

		void blitToDefaultBuffer();

		glm::uvec4 readColorPixel(int col, int row, int slot);

		std::shared_ptr<gl::Texture> getRenderTexture(int slot);

		void readColorAttachment(int slot, int x, int y, int width, int height, void* buffer);

		void bind();
		void unbind();

	private:
		static Framebuffer* s_boundBuffer;

		struct FramebufferAttachment {
			FramebufferAttachment();
			FramebufferAttachment(GLenum attachment, std::shared_ptr<gl::Texture> texture = nullptr);
			FramebufferAttachment(FramebufferAttachment&& other);
			~FramebufferAttachment();

			FramebufferAttachment& operator=(FramebufferAttachment&& other);

			void attach(GLuint framebuffer, int width, int height);

			std::shared_ptr<gl::Texture> targetTexture;
			GLuint targetBuffer;
			GLenum attachment;
		};

		GLuint mId;
		std::vector<FramebufferAttachment> mColorAttachments;
		FramebufferAttachment mDepthAttachment;
		int mWidth, mHeight;
		bool mRequriesUpdate;
	};

}