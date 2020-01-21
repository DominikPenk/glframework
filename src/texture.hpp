#pragma once

#include <glad/glad.h>
#include <imgui.h>

namespace gl {
	class Texture {
	public:
		Texture();
		Texture(int cols, int rows,
			GLenum exposedFormat = GL_RGBA,
			GLenum internalFormat = GL_RGBA,
			GLenum dataType = GL_UNSIGNED_BYTE,
			unsigned char* data = nullptr);
		~Texture();

		inline operator GLuint() const { return id; }
		inline operator ImTextureID() const { return (GLuint*)id; }

		void setData(unsigned char* data, GLenum dataFormat);
		void update();

		void resize(int cols, int rows = -1, int depth = -1);

		void bind(int slot = -1);

		void readPixels(char* data);

		const GLuint& id;
		const int& rows;
		const int& cols;
		const int& depth;
		const int& dimensions;
		const GLenum& exposedFormat;
		const GLenum& internalFormat;
		const GLenum& dataType;
		const GLenum& textureType;
		
		GLenum textureWrapType[3];
		GLenum magFilterType;
		GLenum minFilterType;
		bool generateMipMap;

	protected:
		void init();

		bool mUpdated, mResized;
		unsigned char* mDataPtr;
		GLuint mId;
		int mCols, mRows, mDepth;
		int mDimensions;
		GLenum mTextureType;
		GLenum mExposedFormat;
		GLenum mInternalFormat;
		GLenum mDataType;
	};
}