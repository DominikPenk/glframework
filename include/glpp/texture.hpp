#pragma once

#include <glad/glad.h>
#include "glpp/imgui.hpp"

#include <string>

namespace gl {
	enum class PixelType {
		Float = GL_FLOAT,
		UByte = GL_UNSIGNED_BYTE,
		UInt  = GL_UNSIGNED_INT,
		UIntByte = GL_UNSIGNED_INT_24_8,
	};

	enum class PixelFormat {
		Red = GL_LUMINANCE,
		RG = GL_RG,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
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

	enum TextureFlags {
		With_Mipmap            = (0x0 << 0), // b000000
		No_Mipmap              = (0x1 << 0), // b000001
		
		Filter_Linear          = (0x0 << 1), // b000000
		Filter_Nearest         = (0x1 << 1), // b000010
		Filter_Nearest_Linear  = (0x3 << 1), // b000110
		Filter_Linear_Nearest  = (0x2 << 1), // b000100
		
		Wrap_Clamp             = (0x0 << 3), // b000000
		Wrap_Repeat            = (0x1 << 3), // b001000
		Wrap_Border            = (0x2 << 3), // b010000
		Wrap_Mirrored_Clamp    = (0x3 << 3), // b011000  
		Wrap_Mirrored_Repeat   = (0x4 << 3), // b100000

		Force_Init             = (0x0 << 6),
		Lazy_Init              = (0x1 << 6)
	};

	constexpr GLenum getGLFormat(PixelFormat format) {
		return static_cast<GLenum>(format);
	}
	GLenum getGlSizedFormat(PixelFormat format, PixelType type);

	int getChannelsForFormat(PixelFormat format);

	class Texture {
	public:
		// Creates a 1D-Texture
		Texture(int cols, PixelFormat pixelFormat, gl::PixelType dataType, int flags = 0);
		// Creates a 2D-Texture
		Texture(int cols, int rows, PixelFormat pixelFormat = gl::PixelFormat::RGBA, gl::PixelType dataType = gl::PixelType::UByte, int flags = 0);
		Texture(std::string path, bool flipY = true, TextureFlags flags = (TextureFlags)0);
		// Creates a 3D-Texture
		Texture(int cols, int rows, int depth, PixelFormat pixelFormat, gl::PixelType dataType, int flags = 0);
		
		~Texture();

		inline operator GLuint() const { return id; }
		inline operator ImTextureID() const { return (GLuint*)id; }

		// Setter functions
		void createMipmap(bool shouldCreate);
		void setData(void* data, PixelFormat format = PixelFormat::Default);
		void setFilters(gl::FilterType minFilter, gl::FilterType magFilter);
		void setWrapping(gl::WrapType s, gl::WrapType t = gl::WrapType::None, gl::WrapType r = gl::WrapType::None);
		void resize(int cols, int rows = -1, int depth = -1);

		// Download functions
		void download(void* dst, gl::PixelFormat format = gl::PixelFormat::Default, int level = 0);

		void bind(int slot = -1);
		void unbind();

		// Getters and attributes
		int dimensions() const;
		gl::WrapType wrap(int dim) const;

		gl::FilterType magFilter() const;
		gl::FilterType minFilter() const;
		bool hasMipmap() const;
		int channels() const;

		const GLuint& id;
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
		Texture(TextureType type, PixelFormat pixelFormat, gl::PixelType dataType, int flags);
		void setValuesFromFlags(TextureFlags flags);
		void init();

		GLuint mId;
		FilterType mMagFilterType;
		FilterType mMinFilterType;
		WrapType mWrapType[3];
		int mCols, mRows, mDepth;
		TextureType mTextureType;
		PixelType mDataType;
		PixelFormat mPixelFormat;
		bool mCreateMipmap;
	};
}