#include "glpp/texture.hpp"

#include "3rdparty/stb_image.h"

#include <iostream>

gl::LargeTexture::LargeTexture(int cols, int rows, PixelFormat pixelFormat, gl::PixelType dataType, TextureFlags flags) :
	TextureBase(TextureType::D2, pixelFormat, dataType, flags),
	rowTiles(mRowTiles),
	colTiles(mColTiles)
{
	// We always do a force init (FIXME)
	if (flags & TextureFlags_Lazy_Init) {
		std::cerr << "WARNING: Requested lazy init for a large texture but this is not implemented.\n";
	}

	mRows = rows;
	mCols = cols;
	init();
	
}

gl::LargeTexture::LargeTexture(std::string path, bool flipY, TextureFlags flags) :
	TextureBase(TextureType::D2, PixelFormat::RGB, PixelType::UByte, flags),
	rowTiles(mRowTiles),
	colTiles(mColTiles)
{
	// We always do a force init (FIXME)
	if (flags & TextureFlags_Lazy_Init) {
		std::cerr << "WARNING: Requested lazy init for a large texture but this is not implemented.\n";
	}

	int channels;
	stbi_set_flip_vertically_on_load(flipY);
	unsigned char* data = stbi_load(path.c_str(), &mCols, &mRows, &channels, 0);
	if (data == NULL) {
		throw std::runtime_error("Could not load texture from \"" + path + "\"");
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

	init();
	setData(data);
	stbi_image_free(data);
}

gl::LargeTexture::~LargeTexture()
{
	glDeleteTextures(numTiles(), mIds.data());	// FIXME: Do we need to check that all ids are valid?
	mIds.clear();
}

void gl::LargeTexture::setData(void* data, PixelFormat pixelFormat)
{
	GLenum format = pixelFormat == gl::PixelFormat::Default ? glFormat() : getGLFormat(pixelFormat);
	GLint oldAlign;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlign);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, mCols);
	for (int j = 0; j < mRowTiles; ++j) {
		for (int i = 0; i < mColTiles; ++i) {
			glBindTexture(GL_TEXTURE_2D, mIds[j * mColTiles + i]);
			auto [w, h] = tileSize(i, j);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS, i * MaxTileSize);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, j * MaxTileSize);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, static_cast<GLenum>(mDataType), data);
		}
	}
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, oldAlign);
}

void gl::LargeTexture::setFilters(gl::FilterType minFilter, gl::FilterType magFilter)
{
	for (GLuint id : mIds) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
	}
	mMinFilterType = minFilter;
	mMagFilterType = magFilter;
}

void gl::LargeTexture::setWrapping(gl::WrapType s, gl::WrapType t, gl::WrapType r)
{
	for (GLuint id : mIds) {
		glBindTexture(GL_TEXTURE_2D, id);
		if (s != WrapType::None) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(s));
			mWrapType[0] = s;
		}
		if (t != WrapType::None) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(t));
			mWrapType[1] = t;
		}
		if (r != WrapType::None) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLenum>(r));
			mWrapType[2] = r;
		}
	}
}

int gl::LargeTexture::dimensions() const
{
	return 2;
}

size_t gl::LargeTexture::numTiles() const
{
	return mRowTiles * mColTiles;
}

std::pair<int, int> gl::LargeTexture::tileSize(int i, int j) const
{
	const int i0 = i * MaxTileSize;
	const int i1 = std::min(i0 + MaxTileSize, mCols);
	const int j0 = j * MaxTileSize;
	const int j1 = std::min(j0 + MaxTileSize, mRows);
	return { i1 - i0, j1 - j0 };
}

ImTextureID gl::LargeTexture::getTileID(int i, int j) const
{
	return (ImTextureID)mIds[j * mColTiles + i];
}

ImVec2 gl::LargeTexture::getRelativePos(int i, int j) const
{
	return ImVec2(
		(float)(i * MaxTileSize) / mCols,
		(float)(j * MaxTileSize) / mRows);
}

void gl::LargeTexture::init()
{
	// Compute number of tiles
	mColTiles = (mCols + MaxTileSize - 1) / MaxTileSize;
	mRowTiles = (mRows + MaxTileSize - 1) / MaxTileSize;

	mIds.resize(mColTiles * mRowTiles, 0);
	glGenTextures(mColTiles * mRowTiles, mIds.data());
	// Set size of all images
	for (size_t j = 0; j < mRowTiles; ++j) {
		for (size_t i = 0; i < mColTiles; ++i) {
			glBindTexture(GL_TEXTURE_2D, mIds[j * mColTiles + i]);
			auto [w, h] = tileSize(i, j);
			glTexImage2D(GL_TEXTURE_2D, 0, glSizedFormat(), w, h, 0, glFormat(), static_cast<GLenum>(mDataType), nullptr);
		}
	}

	setFilters(mMinFilterType, mMagFilterType);
	setWrapping(mWrapType[0], mWrapType[1], mWrapType[2]);
}
