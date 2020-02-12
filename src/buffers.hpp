#pragma once
#include <vector>

#include <glad/glad.h>

#include <eigen3/Eigen/Core>

#include <glm/glm.hpp>

#include "framebuffer.hpp"

#include "gl_internal.hpp"

namespace gl {

	class VertexBufferObjectBase {
	public:
		VertexBufferObjectBase(size_t entrySize) :
			mTarget(GL_ARRAY_BUFFER),
			mUsage(GL_STATIC_DRAW),
			mUpdated(true),
			mOldSize(0),
			mEntrySize(entrySize),
			mId() {

		}

		virtual void update() {
			if (mUpdated) {
				glBindBuffer(mTarget, mId);
				glBufferData(mTarget, mEntrySize * size(), NULL, mUsage);
				glBufferSubData(mTarget, 0, mEntrySize * size(), dataPtr());
				glBindBuffer(mTarget, 0);
				mUpdated = false;
			}
		}

		inline GLenum& target() { return mTarget; }
		inline GLenum target() const { return mTarget; }

		inline GLenum& usage() { return mUsage; }
		inline GLenum usage() const { return mUsage; }

		inline void bind() {
			update();
			glBindBuffer(mTarget, mId);
		}

		inline void unbind() {
			glBindBuffer(mTarget, 0);
		}

		inline void setDirty(bool value) {
			mUpdated = value;
		}

		inline GLuint id() const { return mId; }

		virtual std::size_t size() const = 0;

	protected:
		virtual const void* dataPtr() const = 0;

		gl::BufferIndex mId;
		GLenum mTarget, mUsage;
		bool mUpdated;
		std::size_t mOldSize;
		const std::size_t mEntrySize;
	};

	template<typename T, int n>
	class VertexBufferObject : public VertexBufferObjectBase {
	public:

		template<bool Condition, typename Then, typename Else>
		struct ConditionalType { typedef Then type; };
		template<typename Then, typename Else>
		struct ConditionalType<false, Then, Else> { typedef Else type; };
		typedef typename ConditionalType<n == 1, T, Eigen::Matrix<T, n, 1>>::type value_type;
		typedef glm::vec<n, T> glm_value_type;

		typedef typename std::vector<value_type>::iterator iterator;
		typedef typename std::vector<value_type>::const_iterator const_iterator;

		typedef typename std::vector<glm_value_type>::const_iterator glm_const_iterator;

		VertexBufferObject() :
			VertexBufferObjectBase(n * sizeof(T))
		{}
		VertexBufferObject(const std::vector<value_type>& data, GLenum target, GLenum usage = GL_DYNAMIC_DRAW) :
			VertexBufferObjectBase()
		{
			mData = data;
			mTarget = target;
			mUsage = usage;
			mUpdated = true;
		}
		VertexBufferObject(const VertexBufferObject& other) {
			mData = other.mData;
			mUsage = other.mUsage;
			mTarget = other.mTarget;
			mUpdated = true;
			mId = 0;
		}
		VertexBufferObject(VertexBufferObject&& other) {
			mData = std::move(other.mData);
			mUsage = other.mUsage;
			mTarget = other.mTarget;
			mUpdated = other.mUpdated;
			mId = other.mId;
			// Delte other data
			other.mId = 0;
		}

		VertexBufferObject& operator=(const std::vector<value_type>& data) {
			mData.resize(data.size());
			std::copy(data.begin(), data.end(), mData.begin());
			mUpdated = true;
			return *this;
		}

		VertexBufferObject& operator=(const VertexBufferObject& other) {
			if (this != &other) {
				mData.resize(other.mData.size());
				std::copy(other.mData.begin(), other.mData.end(), mData.begin());
				mUsage = other.mUsage;
				mTarget = other.mTarget;
				mId = 0;
				mUpdated = true;
			}
			return *this;
		}

		VertexBufferObject& operator=(VertexBufferObject&& other) {
			if (this != &other) {
				mData = std::move(other.mData);
				mUsage = other.mUsage;
				mTarget = other.mTarget;
				mUpdated = other.mUpdated;
				mId = other.mId;
				// Delte other data
				other.mId = 0;
			}
			return *this;
		}

		void push_back(const value_type& element) { mData.push_back(element); mUpdated = true; }

		void push_back(const glm_value_type& element) {
			static_assert(n <= 4, "only valid for sizes less than 5");
			if constexpr (n == 1) {
				mData.push_back(element[0]);
			}
			else {
				mData.push_back(Eigen::Map<const value_type>(&element[0]));
			}
			mUpdated = true;
		}

		void insert(const_iterator position, std::initializer_list<value_type> data) {
			mUpdated = true;
			mData.insert(position, data);
		}
		void insert(const_iterator position, const_iterator start, const_iterator end) {
			mUpdated = true;
			mData.insert(position, start, end);
		}
		
		void extend(glm_const_iterator start, glm_const_iterator end) {
			mUpdated = true;
			for (auto it = start; it != end; it = std::next(it)) {
				push_back(*it);
			}
		}
		void extend(std::initializer_list<glm_value_type> data) {
			mUpdated = true;
			for (const glm_value_type& e : data) {
				push_back(e);
			}
		}

		void erase(const_iterator position) { mData.erase(position); }
		void erase(const_iterator first, const_iterator last) { mData.erase(first, last); }

		inline value_type& at(size_t i) {
			mUpdated = true;
			return mData.at(i);
		}
		inline const value_type& at(size_t i) const { return mData.at(i); }

		inline value_type& operator[](size_t i) { mUpdated = true; return mData[i]; }
		inline const value_type& operator[](size_t i) const { return mData[i]; }

		inline value_type& front() { mUpdated = true; return mData.front(); }
		inline const value_type& front() const { mUpdated = true; return mData.front(); }

		inline value_type& back() { mUpdated = true; return mData.back(); }
		inline const value_type& back() const { mUpdated = true; return mData.back(); }

		inline iterator begin() { return mData.begin(); }
		inline const_iterator begin() const { return mData.begin(); }

		inline iterator end() { return mData.end(); }
		inline const_iterator end() const { return mData.end(); }

		inline bool empty() const { return mData.empty(); }
		inline size_t size() const override { return mData.size(); }
		inline const std::vector<value_type>& vector() const { return mData; }
		inline const value_type* data() const { return mData.data(); }
		inline value_type* data() { return mData.data(); }

		inline void resize(size_t size) {
			mData.resize(size);
			mUpdated = true;
		}
		inline void resize(size_t size, const value_type& val) {
			mData.resize(size, val);
			mUpdated = true;
		}
		inline void clear() {
			mUpdated = true;
			mData.clear();
		}

		/// Python style Array indexing
		value_type& operator()(int i) {
			mUpdated = true;
			if (i < 0) { return mData[mData.size() - i]; }
			else return mData[i];
		}
		/// Python style Array indexing
		const value_type& operator()(int i) const {
			//mUpdated = true;
			if (i < 0) { return mData[mData.size() - i]; }
			else return mData[i];
		}

	protected:
		virtual const void* dataPtr() const {
			return reinterpret_cast<const void*>(mData.data());
		}

	private:
		std::vector<value_type> mData;
	};

#pragma region typedefs
	typedef VertexBufferObject<unsigned int, 1> IndexBuffer;
#pragma endregion
}

#include "variadic_buffer.hpp"

namespace gl {

	class VertexArrayObject {
	public:
		VertexArrayObject() :
			mId(0),
			indices(nullptr)
		{}

		~VertexArrayObject() {
			glDeleteBuffers(1, &mId);
			mId = 0;
		}

		template<typename T, int n>
		void addVertexAttribute(
			VertexBufferObject<T, n>& buffer,
			GLuint index,
			GLint size = n,
			GLuint offset = 0,
			GLenum type = impl::toGLenum<T>(),
			GLboolean  normalized = false) {

			// Check if VAO has to be initialized
			if (0 == mId) {
				glGenVertexArrays(1, &mId);
			}
			glBindVertexArray(mId);
			buffer.bind();
			glEnableVertexAttribArray(index);
			if (type == GL_INT || type == GL_UNSIGNED_INT) {
				glVertexAttribIPointer(index, size, type, sizeof(T) * n, reinterpret_cast<void*>(offset));
			}
			else {
				glVertexAttribPointer(index, size, type, normalized, sizeof(T) * n, reinterpret_cast<GLvoid*>(offset));
			}
			glEnableVertexAttribArray(index);
			// Clean up
			buffer.unbind();
			glBindVertexArray(0);

			buffers.push_back(&buffer);
		}

		template<typename T, int n>
		void addVertexAttribute(
			std::shared_ptr<VertexBufferObject<T, n>> buffer,
			GLuint index,
			GLint size = n,
			GLuint offset = 0,
			GLenum type = impl::toGLenum<T>(),
			GLboolean  normalized = false) {

			// Check if VAO has to be initialized
			if (0 == mId) {
				glGenVertexArrays(1, &mId);
			}
			glBindVertexArray(mId);
			buffer->bind();
			glEnableVertexAttribArray(index);
			if (type == GL_INT || type == GL_UNSIGNED_INT) {
				glVertexAttribIPointer(index, size, type, sizeof(T) * n, reinterpret_cast<void*>(offset));
			}
			else {
				glVertexAttribPointer(index, size, type, normalized, sizeof(T) * n, reinterpret_cast<GLvoid*>(offset));
			}
			glEnableVertexAttribArray(index);
			// Clean up
			buffer->unbind();
			glBindVertexArray(0);

			sharedBuffers.push_back(buffer);
		}

		template<int element, typename ...Args>
		void addVertexAttribute(
			std::shared_ptr<CompactVertexBufferObject<Args...>> buffer,
			GLuint index,
			GLboolean normalize = GL_FALSE,
			GLuint size = 0,
			GLenum type = 0) {

			using T = typename CompactVertexBufferObject<Args...>::value_type;
			
			if (0 == mId) {
				glGenVertexArrays(1, &mId);
			}
			glBindVertexArray(mId);
			buffer->bind();
			glEnableVertexAttribArray(index);
			size = size == 0 ? impl::to_size_v<std::tuple_element_t<element, T>> : size;
			type = type == 0 ? impl::to_glenum_v<std::tuple_element_t<element, T>> : type;
			std::ptrdiff_t offset = impl::tuple_inner_offset<element, T>();
			if (type == GL_INT || type == GL_UNSIGNED_INT ) {
				glVertexAttribIPointer(index, size, type, sizeof(T), reinterpret_cast<void*>(offset));
			}
			else {
				glVertexAttribPointer(index, 
					size,
					type,
					normalize, 
					sizeof(T), 
					reinterpret_cast<void *>(offset));
			}
			glEnableVertexAttribArray(index);
			// Clean up
			buffer->unbind();
			glBindVertexArray(0);

			sharedBuffers.push_back(buffer);
		}

		template<typename ...Args>
		void addAllVertexAttributes(
			std::shared_ptr<CompactVertexBufferObject<Args...>> buffer,
			GLboolean normalize = GL_FALSE) {

			using T = typename CompactVertexBufferObject<Args...>::value_type;

			if (0 == mId) {
				glGenVertexArrays(1, &mId);
			}
			glBindVertexArray(mId);
			buffer->bind();
			impl::AddVertexAttribute<0, T>(normalize, 0);
			buffer->unbind();
			glBindVertexArray(0);

			sharedBuffers.push_back(buffer);
		}

		void setIndexBufferObject(VertexBufferObject<unsigned int, 1>& buffer) {
			if (buffer.target() != GL_ELEMENT_ARRAY_BUFFER) throw std::invalid_argument("Buffer has to have the target GL_ELEMENT_ARRAY_BUFFER");
			// Check if VAO has to be initialized
			if (0 == mId) {
				glGenVertexArrays(1, &mId);
			}
			glBindVertexArray(mId);
			buffer.bind();
			glBindVertexArray(0);
			buffer.unbind();
			indices = &buffer;
		}

		inline void bind() {
			if (mId == 0) {
				glGenVertexArrays(1, &mId);
			}
			if (indices != nullptr) indices->update();
			for (auto buffer : sharedBuffers)
				buffer->update();
			glBindVertexArray(mId);
		}
		inline void unbind() {
			glBindVertexArray(0);
		}

		void draw(int n = -1) {
			// Update all buffers
			for (auto buffer : buffers) {
				buffer->update();
			}
			if (indices != nullptr) {
				indices->update();
			}
			bind();
			if (indices == 0 && n < 0) {

			}
			unbind();
		}
	private:
		GLuint mId;
		VertexBufferObject<unsigned int, 1>* indices;
		std::vector<VertexBufferObjectBase*> buffers;
		std::vector<std::shared_ptr<VertexBufferObjectBase>> sharedBuffers;
		int n;
	};
}
