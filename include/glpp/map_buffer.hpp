#pragma once

namespace gl {

	template<typename T>
	class VertexBufferObjectMap : public VertexBufferObjectBase {
	public:
		struct AttribInfo {
			int count;
			GLenum type;
			bool normalize;
			ptrdiff_t offset;
		};

		VertexBufferObjectMap(T* data, size_t size, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW) :
			VertexBufferObjectBase(sizeof(T)),
			mDataPtr(data),
			mSize(size)
		{
			mUsage = usage;
			mTarget = target;
			mUpdated = true;
		}

		void setLayout(std::vector<AttribInfo> layout) { vertexAttributes = layout; }

		virtual std::size_t size() const override
		{
			return mSize;
		}
		virtual void clear() override
		{
			throw std::runtime_error("Cannot clear VertexBufferObjectMap");
		}

		void bufferData(T* data, size_t count) {
			mDataPtr = data;
			mSize = count;
			bind();
			glBufferData(mTarget, mSize * sizeof(T), (const GLvoid*)data, mUsage);
		}

		std::vector<AttribInfo> vertexAttributes;


	protected:
		virtual const void* dataPtr() const override
		{
			return mDataPtr;
		}



	private:
		T* mDataPtr;
		size_t mSize;

	};

}
