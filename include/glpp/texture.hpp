#pragma once

#include <glad/glad.h>
#include "glpp/imgui.hpp"

#include <string>

namespace gl {
	typedef int TextureFlags;

	enum class PixelType {
		Float = GL_FLOAT,
		UByte = GL_UNSIGNED_BYTE,
		UInt  = GL_UNSIGNED_INT,
		UIntByte = GL_UNSIGNED_INT_24_8,
	};

	enum class PixelFormat {
		Red = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		DEPTH_STENCIL = GL_DEPTH_STENCIL,
		DEPTH = GL_DEPTH_COMPONENT,
		Default = 0
	};

	enum class TextureType {
		D1 = GL_TEXTURE_1D,
		D2 = GL_TEXTURE_2D,
		D3 = GL_TEXTURE_3D
	};

	enum class WrapType {
		Clamp = GL_CLAMP_TO_EDGE,
		Border = GL_CLAMP_TO_BORDER,
		Repeat = GL_REPEAT,
		MirroredClamp = GL_MIRROR_CLAMP_TO_EDGE,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		None = 0
	};

	enum class FilterType {
		Linear  = GL_LINEAR,
		Nearest = GL_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		None = 0
	};

	enum TextureFlags_ {
		TextureFlags_With_Mipmap            = (0x0 << 0), // b000000
		TextureFlags_No_Mipmap              = (0x1 << 0), // b000001
		
		TextureFlags_Filter_Linear          = (0x0 << 1), // b000000
		TextureFlags_Filter_Nearest         = (0x1 << 1), // b000010
		TextureFlags_Filter_Nearest_Linear  = (0x3 << 1), // b000110
		TextureFlags_Filter_Linear_Nearest  = (0x2 << 1), // b000100
		
		TextureFlags_Wrap_Clamp             = (0x0 << 3), // b000000
		TextureFlags_Wrap_Repeat            = (0x1 << 3), // b001000
		TextureFlags_Wrap_Border            = (0x2 << 3), // b010000
		TextureFlags_Wrap_Mirrored_Clamp    = (0x3 << 3), // b011000  
		TextureFlags_Wrap_Mirrored_Repeat   = (0x4 << 3), // b100000

		TextureFlags_Force_Init             = (0x0 << 6),
		TextureFlags_Lazy_Init              = (0x1 << 6),

		TextureFlags_FrameBuffer_Texture    = TextureFlags_No_Mipmap | TextureFlags_Filter_Nearest  // This is a shorthand to create textures ready to use in a framebuffer
	};

	constexpr GLenum getGLFormat(PixelFormat format) {
		return static_cast<GLenum>(format);
	}
	GLenum getGlSizedFormat(PixelFormat format, PixelType type);

	int getChannelsForFormat(PixelFormat format);

	class TextureBase {
	public:
		TextureBase(TextureType type, PixelFormat pixelFormat, PixelType dataType, TextureFlags flags);

		// Setters
		virtual void setData(void* data, PixelFormat format = PixelFormat::Default) = 0;
		virtual void setFilters(gl::FilterType minFilter, gl::FilterType magFilter) = 0;
		virtual void setWrapping(gl::WrapType s, gl::WrapType t = gl::WrapType::None, gl::WrapType r = gl::WrapType::None) = 0;
		
		// Getters
		virtual int dimensions() const;
		int channels() const;
		virtual gl::WrapType wrap(int dim) const;
		virtual gl::FilterType magFilter() const;
		virtual gl::FilterType minFilter() const;

		// Properties
		const int& rows;
		const int& cols;
		const int& depth;
		const PixelFormat& pixelFormat;
		const PixelType& pixelType;
		const TextureType& type;

		// These are low level function you should not need
		GLenum glTextureType() const;
		GLenum glType() const;
		GLenum glSizedFormat() const;
		GLenum glFormat() const;
	protected:
		FilterType mMagFilterType;
		FilterType mMinFilterType;
		WrapType mWrapType[3];
		int mCols, mRows, mDepth;
		TextureType mTextureType;
		PixelType mDataType;
		PixelFormat mPixelFormat;
	};

	class Texture : public TextureBase {
	public:
		// Creates a 1D-Texture
		Texture(int cols, PixelFormat pixelFormat, gl::PixelType dataType, TextureFlags flags = 0);
		// Creates a 2D-Texture
		Texture(int cols, int rows, PixelFormat pixelFormat = gl::PixelFormat::RGBA, gl::PixelType dataType = gl::PixelType::UByte, TextureFlags flags = 0);
		Texture(std::string path, bool flipY = true, TextureFlags flags = 0);
		// Creates a 3D-Texture
		Texture(int cols, int rows, int depth, PixelFormat pixelFormat, gl::PixelType dataType, TextureFlags flags = 0);
		
		~Texture();

		inline operator GLuint() const { return id; }
		inline operator ImTextureID() const { return (GLuint*)id; }

		// Implemented and new Setter functions
		void createMipmap(bool shouldCreate);
		virtual void setData(void* data, PixelFormat format = PixelFormat::Default) override;
		virtual void setFilters(gl::FilterType minFilter, gl::FilterType magFilter) override;
		virtual void setWrapping(gl::WrapType s, gl::WrapType t = gl::WrapType::None, gl::WrapType r = gl::WrapType::None) override;
		void resize(int cols, int rows = -1, int depth = -1);

		// Download functions
		void download(void* dst, gl::PixelFormat format = gl::PixelFormat::Default, int level = 0);

		void bind(int slot = -1);
		void unbind();

		// New getters
		bool hasMipmap() const;

		const GLuint& id;

	protected:
		Texture(TextureType type, PixelFormat pixelFormat, gl::PixelType dataType, TextureFlags flags);
		void init();

		GLuint mId;
		bool mCreateMipmap;
	};

	// Implements a texture that is too large to be stored in a single OpenGL texture
	// Currently only implemented for 2D-Textures
	class LargeTexture : public TextureBase {
	public:
		constexpr static int MaxTileSize = 2048;

		LargeTexture(int cols, int rows, PixelFormat pixelFormat = gl::PixelFormat::RGBA, gl::PixelType dataType = gl::PixelType::UByte, TextureFlags flags = 0);
		LargeTexture(std::string path, bool flipY = true, TextureFlags flags = 0);

		~LargeTexture();

		// Inherited via TextureBase
		virtual void setData(void* data, PixelFormat format = PixelFormat::Default) override;
		virtual void setFilters(gl::FilterType minFilter, gl::FilterType magFilter) override;
		virtual void setWrapping(gl::WrapType s, gl::WrapType t = gl::WrapType::None, gl::WrapType r = gl::WrapType::None) override;
		virtual int dimensions() const override;

		// New getters
		size_t numTiles() const;
		std::pair<int, int> tileSize(int i, int j) const;

		ImTextureID getTileID(int i, int j) const;
		ImVec2 getRelativePos(int i, int j) const;

		// Properties
		int& rowTiles;
		int& colTiles;

	protected:
		void init();

		std::vector<GLuint> mIds;
		int mRowTiles, mColTiles;

	};
}