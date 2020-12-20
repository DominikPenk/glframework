#include "glpp/texture.hpp"

#include <stdexcept>
#include <string>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"

namespace impl {
	GLenum textureType(int dimensions) {
		switch (dimensions) {
		case 1:
			return GL_TEXTURE_1D;
		case 2:
			return GL_TEXTURE_2D;
		case 3:
			return GL_TEXTURE_3D;
		default:
			throw std::invalid_argument(
				"Invalid number of texture dimensions was " 
				+ std::to_string(dimensions) 
				+ " but should be in range [1, 3]");
		}
	}

	GLenum getInternalByteFormat(int channels) {
		switch (channels) {
		case 1:
			return GL_R8;
		case 2:
			return GL_RG8;
		case 3:
			return GL_RGB8;
		case 4:
			return GL_RGBA8;
		}
	}

	GLenum getFormat(int channels) {
		switch (channels) {
		case 1:
			return GL_LUMINANCE;
		case 2:
			return GL_RG;
		case 3:
			return GL_RGB;
		case 4:
			return GL_RGBA;
		}
	}
}


GLenum gl::Texture::glTextureType() const
{
	return static_cast<GLenum>(mTextureType);
}

GLenum gl::Texture::glType() const
{
	return static_cast<GLenum>(mDataType);
}

GLenum gl::Texture::glSizedFormat() const
{
	return getGlSizedFormat(mPixelFormat, mDataType);
}

GLenum gl::Texture::glFormat() const
{
	return static_cast<GLenum>(mPixelFormat);
}

gl::Texture::Texture(TextureType type, PixelFormat pixelFormat, gl::PixelType dataType, int flags) :
	mTextureType(type),
	mPixelFormat(pixelFormat),
	mDataType(dataType),
	mId(0),
	id(mId),
	rows(mRows),
	cols(mCols),
	depth(mDepth),
	pixelFormat(mPixelFormat),
	pixelType(mDataType),
	type(mTextureType)
{
	setValuesFromFlags((gl::TextureFlags)flags);
}

gl::Texture::Texture(int cols, PixelFormat pixelFormat, gl::PixelType dataType, int flags) :
	Texture(TextureType::D1, pixelFormat, dataType, flags)
{
	mCols = cols;
	mRows = mDepth = 1;
	if (flags & Lazy_Init == Force_Init) {
		init();
	}
}

gl::Texture::Texture(int cols, int rows, PixelFormat pixelFormat, gl::PixelType dataType, int flags) :
	Texture(TextureType::D2, pixelFormat, dataType, flags)
{
	mCols = cols;
	mRows = rows;
	mDepth = 1;

	if (flags & Lazy_Init == Force_Init) {
		init();
	}
}

gl::Texture::Texture(int cols, int rows, int depth, PixelFormat pixelFormat, gl::PixelType dataType, int flags) :
	Texture(TextureType::D3, pixelFormat, dataType, flags)
{
	mCols = cols;
	mRows = rows;
	mDepth = depth;
	if (flags & Lazy_Init == Force_Init) {
		init();
	}
}

gl::Texture::Texture(std::string path, bool flipY, TextureFlags flags) :
	Texture(TextureType::D2, PixelFormat::RGB, PixelType::UByte, flags)
{
	int channels;
	unsigned char * data = stbi_load(path.c_str(), &mCols, &mRows, &channels, 0);
	if(data == NULL) {
		throw std::runtime_error("Could not load texture from \"" + path + "\"");
	}
	if (flipY) {
		for (int j = 0; j < mRows / 2; ++j) {
			for (int i = 0; i < mCols; ++i) {
				for (int c = 0; c < channels; ++c) {
					std::swap(data[channels * (j * mCols + i) + c], data[channels * ((mRows - 1 - j) * mCols + i) + c]);
				}
			}
		}
	}
	if (channels == 1) {
		mPixelFormat = PixelFormat::Red;
	}
	else if (channels == 2) {
		mPixelFormat = PixelFormat::RG;
	}
	else if (channels == 4) {
		mPixelFormat = PixelFormat::RGBA;
	}

	setData(data);
	stbi_image_free(data);
}

gl::Texture::~Texture()
{
	if (mId != 0) {
		glDeleteTextures(1, &mId);
		mId = 0;
	}
}

void gl::Texture::setValuesFromFlags(TextureFlags _flags)
{
	mCreateMipmap = (static_cast<int>(_flags) & No_Mipmap) == 0x0;
	int filterType = _flags & Filter_Nearest_Linear;
	int wrapType = _flags & (0x7 << 3);

	switch (wrapType) {
	case Wrap_Clamp:
		mWrapType[0] = mWrapType[1] = mWrapType[2] = gl::WrapType::Clamp;
		break;
	case Wrap_Repeat:
		mWrapType[0] = mWrapType[1] = mWrapType[2] = gl::WrapType::Repeat;
		break;
	case Wrap_Border:
		mWrapType[0] = mWrapType[1] = mWrapType[2] = gl::WrapType::Border;
		break;
	case Wrap_Mirrored_Clamp:
		mWrapType[0] = mWrapType[1] = mWrapType[2] = gl::WrapType::MirroredClamp;
		break;
	case Wrap_Mirrored_Repeat:
		mWrapType[0] = mWrapType[1] = mWrapType[2] = gl::WrapType::MirroredRepeat;
		break;
	}

	switch (filterType) {
	case Filter_Linear:
		mMagFilterType = mMinFilterType = gl::FilterType::Linear;
		break;
	case Filter_Nearest:
		mMagFilterType = mMinFilterType = gl::FilterType::Nearest;
		break;
	case Filter_Nearest_Linear:
		mMagFilterType = mMinFilterType = gl::FilterType::NearestMipmapLinear;
		break;
	case Filter_Linear_Nearest:
		mMagFilterType = mMinFilterType = gl::FilterType::LinearMipmapNearest;
		break;
	}
}

void gl::Texture::createMipmap(bool shouldCreate)
{
	if (shouldCreate) {
		bind();
		glGenerateMipmap(static_cast<GLenum>(mTextureType));
	}
	mCreateMipmap = shouldCreate;
}

void gl::Texture::setData(void* data, PixelFormat pixelFormat)
{
	bind();
	GLenum format = pixelFormat == gl::PixelFormat::Default ? glFormat() : getGLFormat(pixelFormat);

	switch (mTextureType) {
	case TextureType::D1:
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, mCols, format, static_cast<GLenum>(mDataType), data);
		break;
	case TextureType::D2:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mCols, mRows, format, static_cast<GLenum>(mDataType), data);
		break;
	case TextureType::D3:
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mCols, mRows, mDepth, format, static_cast<GLenum>(mDataType), data);
		break;
	default:
		throw std::runtime_error("Invalid number of dimensions");
	}
	createMipmap(mCreateMipmap);
	glBindTexture(static_cast<GLenum>(mTextureType), 0);
}

void gl::Texture::setFilters(gl::FilterType minFilter, gl::FilterType magFilter)
{
	bind();
	glTexParameteri(static_cast<GLenum>(mTextureType), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
	glTexParameteri(static_cast<GLenum>(mTextureType), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
	mMinFilterType = minFilter;
	mMagFilterType = magFilter;
}

void gl::Texture::setWrapping(gl::WrapType s, gl::WrapType t, gl::WrapType r)
{
	bind();
	if (s != WrapType::None) {
		glTexParameteri(static_cast<GLenum>(mTextureType), GL_TEXTURE_WRAP_S, static_cast<GLenum>(s));
		mWrapType[0] = s;
	}
	if (t != WrapType::None) {
		glTexParameteri(static_cast<GLenum>(mTextureType), GL_TEXTURE_WRAP_T, static_cast<GLenum>(t));
		mWrapType[1] = t;
	}
	if (r != WrapType::None) {
		glTexParameteri(static_cast<GLenum>(mTextureType), GL_TEXTURE_WRAP_R, static_cast<GLenum>(r));
		mWrapType[2] = r;
	}
}

void gl::Texture::resize(int cols, int rows, int depth)
{
	bind();
	mCols = cols;
	if (rows > 0) {
		mRows = rows;
	}
	if (depth > 0) {
		mDepth = depth;
	}

	switch (mTextureType) {
	case TextureType::D1:
		glTexImage1D(GL_TEXTURE_1D, 0, glSizedFormat(), mCols, 0, glFormat(), static_cast<GLenum>(mDataType), nullptr);
		break;
	case TextureType::D2:
		glTexImage2D(GL_TEXTURE_2D, 0, glSizedFormat(), mCols, mRows, 0, glFormat(), static_cast<GLenum>(mDataType), nullptr);
		break;
	case TextureType::D3:
		glTexImage3D(GL_TEXTURE_3D, 0, glSizedFormat(), mCols, mRows, mDepth, 0, glFormat(), static_cast<GLenum>(mDataType), nullptr);
		break;
	}
}

void gl::Texture::download(void* dst, gl::PixelFormat _format, int level)
{
	bind();
	GLenum format = _format == gl::PixelFormat::Default ? glFormat() : getGLFormat(_format);
	glGetTexImage(static_cast<GLenum>(mTextureType), level, format, static_cast<GLenum>(mDataType), dst);
}

void gl::Texture::bind(int slot)
{
	if (mId == 0) {
		init();
	}
	if (slot >= 0) {
		glActiveTexture(GL_TEXTURE0 + slot);
	}
	glBindTexture(static_cast<GLenum>(mTextureType), mId);
}

void gl::Texture::unbind()
{
	glBindTexture(static_cast<GLenum>(mTextureType), 0);
}

int gl::Texture::dimensions() const
{
	switch (mTextureType) {
	case TextureType::D1:
		return 1;
	case TextureType::D2: 
		return 2;
	case TextureType::D3:
		return 3;
	default:
		throw std::runtime_error("Invalid texture type");
	}
}

gl::WrapType gl::Texture::wrap(int dim) const
{
	assert(dim >= 0 && dim <= 3);
	return mWrapType[dim];
}

gl::FilterType gl::Texture::magFilter() const
{
	return mMagFilterType;
}

gl::FilterType gl::Texture::minFilter() const
{
	return mMinFilterType;
}

bool gl::Texture::hasMipmap() const
{
	return mCreateMipmap;
}

int gl::Texture::channels() const
{
	return getChannelsForFormat(mPixelFormat);
}

void gl::Texture::init()
{
	glGenTextures(1, &mId);
	resize(mCols, mRows, mDepth);
	setFilters(mMinFilterType, mMagFilterType);
	setWrapping(mWrapType[0], mWrapType[1], mWrapType[2]);
	createMipmap(mCreateMipmap);
}

GLenum gl::getGlSizedFormat(PixelFormat pixelFormat, PixelType pixelType)
{
	if (pixelType == PixelType::Float) {
		switch (pixelFormat) {
		case PixelFormat::BGR:
		case PixelFormat::RGB:
			return GL_RGB32F;
		case PixelFormat::Red:
			return GL_R32F;
		case PixelFormat::RG:
			return GL_RG32F;
		case PixelFormat::RGBA:
			return GL_RGBA32F;
		case PixelFormat::DEPTH:
			return GL_DEPTH_COMPONENT24;
		default:
			throw std::runtime_error("Invalid dataype and pixel format combination");
		}
	}
	else if (pixelType == PixelType::UByte) {
		switch (pixelFormat) {
		case PixelFormat::BGR:
		case PixelFormat::RGB:
			return GL_RGB8;
		case PixelFormat::Red:
			return GL_R8;
		case PixelFormat::RG:
			return GL_RG8;
		case PixelFormat::RGBA:
			return GL_RGBA8;
		default:
			throw std::runtime_error("Invalid dataype and pixel format combination");
		}
	}
	else if (pixelType == PixelType::UInt) {
		switch (pixelFormat) {
		case PixelFormat::BGR:
		case PixelFormat::RGB:
			return GL_RGBA32UI;
		case PixelFormat::Red:
			return GL_R32UI;
		case PixelFormat::RG:
			return GL_RG32UI;
		case PixelFormat::RGBA:
			return GL_RGBA32UI;
		default:
			throw std::runtime_error("Invalid dataype and pixel format combination");
		}
	}
	else if (pixelType == PixelType::UIntByte) {
		if (pixelFormat == PixelFormat::DEPTH_STENCIL) {
			return GL_DEPTH24_STENCIL8;
		}
		else
		{
			throw std::runtime_error("Invalid dataype and pixel format combination");
		}
	}

	throw std::runtime_error("Unknown data type");
}

int gl::getChannelsForFormat(PixelFormat format)
{
	switch (format) {
	case PixelFormat::RGBA:
		return 4;
	case PixelFormat::BGR:
	case PixelFormat::RGB:
		return 4;
	case PixelFormat::RG:
		return 2;
	case PixelFormat::DEPTH:
	case PixelFormat::Red:
	case PixelFormat::DEPTH_STENCIL:
		return 1;
	default:
		std::invalid_argument("Invalid format");
	}
}
