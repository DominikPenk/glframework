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

gl::Texture::Texture() :
	id(mId),
	cols(mCols),
	rows(mRows),
	exposedFormat(mExternalFormat),
	depth(mDepth),
	dimensions(mDimensions),
	dataType(mDataType),
	textureType(mTextureType),
	internalFormat(mInternalFormat),
	mCols(0),
	mRows(0),
	mDepth(0),
	mDimensions(2),
	mExternalFormat(GL_RGBA),
	mId(0),
	mUpdated(true),
	mResized(false),
	mDataPtr(nullptr),
	mTextureType(impl::textureType(dimensions)),
	mInternalFormat(GL_RGBA),
	mDataType(GL_UNSIGNED_BYTE)
{
	textureWrapType[0] = textureWrapType[1] = textureWrapType[2] = GL_CLAMP_TO_EDGE;
	magFilterType = GL_LINEAR;
	minFilterType = GL_LINEAR;
	generateMipMap = true;
}

gl::Texture::Texture(int cols, int rows,
	GLenum exposedFormat,
	GLenum internalFormat,
	GLenum dataType,
	unsigned char* data) :
	Texture()
{
	mExternalFormat = exposedFormat;
	mInternalFormat = internalFormat;
	mDataType = dataType;
	mUpdated = true;
	mResized = true;
	mRows = rows;
	mCols = cols;
	if (data != nullptr) {
		setData(data, dataType);
	}
}

gl::Texture::Texture(std::string path, bool flipY) :
	Texture()
{
	int channels;
	mDataPtr = stbi_load(path.c_str(), &mCols, &mRows, &channels, 0);
	if(mDataPtr == NULL) {
		throw std::runtime_error("Could not load texture from \"" + path + "\"");
	}
	if (flipY) {
		for (int j = 0; j < mRows / 2; ++j) {
			for (int i = 0; i < mCols; ++i) {
				for (int c = 0; c < channels; ++c) {
					std::swap(mDataPtr[channels * (j * mCols + i) + c], mDataPtr[channels * ((mRows - 1 - j) * mCols + i) + c]);
				}
			}
		}
	}
	mInternalFormat = impl::getInternalByteFormat(channels);
	mExternalFormat = impl::getFormat(channels);
	setData(mDataPtr, mExternalFormat);
	stbi_image_free(mDataPtr);
}

gl::Texture::~Texture()
{
	if (mId != 0) {
		glDeleteTextures(1, &mId);
		mId = 0;
	}
}

void gl::Texture::setData(unsigned char* data, GLenum dataFormat)
{
	mUpdated = true;
	update();
	glBindTexture(mTextureType, mId);
	switch (mDimensions) {
	case 1:
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, mCols, dataFormat, mDataType, data);
		break;
	case 2:
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mCols, mRows, dataFormat, mDataType, data);
		break;
	case 3:
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mCols, mRows, mDepth, dataFormat, mDataType, data);
	default:
		throw std::runtime_error("Invalid number of dimensions");
	}
	if (generateMipMap) {
		glGenerateMipmap(mTextureType);
	}
	mDataPtr = nullptr;
	glBindTexture(mTextureType, 0);
}

void gl::Texture::update()
{
	if (mUpdated || mResized) {
		if (mId == 0) {
			init();
		}

		glBindTexture(mTextureType, mId);
		// Update texture data
		if (mResized) {
			switch (mDimensions) {
			case 1:
				glTexImage1D(GL_TEXTURE_1D, 0, mInternalFormat, mCols, 0, mExternalFormat, mDataType, nullptr);
				break;
			case 2:
				glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mCols, mRows, 0, mExternalFormat, mDataType, nullptr);
				break;
			case 3:
				glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mCols, mRows, mDepth, 0, mExternalFormat, mDataType, nullptr);
				break;
			default:
				throw std::runtime_error("Invalid number of dimensions");
			}
		}
		glBindTexture(mTextureType, 0);
		mUpdated = false;
		mResized = false;
	}
}

void gl::Texture::resize(int cols, int rows, int depth)
{
	mResized = false;
	switch (mDimensions) {
	case 3:
		mResized |= depth != mDepth;
		mDepth = depth;
	case 2:
		mResized |= rows != mRows;
		mRows = rows;
	case 1:
		mResized |= cols != mCols;
		mCols = cols;
		break;
	default:
		throw std::runtime_error("Invalid number of dimensons");
	}
}

void gl::Texture::bind(int slot)
{
	update();
	if (slot >= 0) {
		glActiveTexture(GL_TEXTURE0 + slot);
	}
	glBindTexture(mTextureType, mId);
}

void gl::Texture::readPixels(char* data)
{
	assert(dimensions == 2);
	bind();
	glGetTexImage(GL_TEXTURE_2D, 0, mInternalFormat, mDataType, data);
	
}



void gl::Texture::init()
{
	glGenTextures(1, &mId);
	glBindTexture(mTextureType, mId);

	switch (mDimensions) {
	case 1:
		glTexImage1D(GL_TEXTURE_1D, 0, mInternalFormat, mCols, 0, mExternalFormat, mDataType, nullptr);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mCols, mRows, 0, mExternalFormat, mDataType, nullptr);
		break;
	case 3:
		glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mCols, mRows, mDepth, 0, mExternalFormat, mDataType, nullptr);
		break;
	default:
		throw std::runtime_error("Invalid number of dimensions");
	}

	glTexParameteri(mTextureType, GL_TEXTURE_WRAP_S, textureWrapType[0]);
	glTexParameteri(mTextureType, GL_TEXTURE_WRAP_T, textureWrapType[1]);
	glTexParameteri(mTextureType, GL_TEXTURE_WRAP_R, textureWrapType[1]);

	glTexParameteri(mTextureType, GL_TEXTURE_MIN_FILTER, minFilterType);
	glTexParameteri(mTextureType, GL_TEXTURE_MAG_FILTER, magFilterType);
	
	glBindTexture(mTextureType, 0);
}
