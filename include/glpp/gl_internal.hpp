#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <functional>
#include <iostream>
#include <type_traits>
#include <memory>

namespace gl {

	class Shader;

	namespace impl {
		template<typename T, typename... Args>
		void SetUniformsHelper(gl::Shader& shader, int nextFreeTextureSlot, const std::string& location, T& value, Args&... rest) {
			if constexpr (std::is_same_v<gl::Texture, T>) {
				shader.bindTexture(location, nextFreeTextureSlot, value);
				nextFreeTextureSlot++;
			}
			else if constexpr (std::is_same_v<std::shared_ptr<gl::Texture>, T>) {
				shader.bindTexture(location, nextFreeTextureSlot, *value);
				nextFreeTextureSlot++;
			}
			else {
				shader.setUniform(location, value);
			}
			if constexpr (sizeof...(rest) > 0) {
				SetUniformsHelper(shader, nextFreeTextureSlot, rest...);
			}
		}

		template<typename... Args>
		void SetUniforms(std::shared_ptr<gl::Shader> shader, Args&... rest) {
			SetUniformsHelper(*shader, 0, rest...);
		}

		template<typename... Args>
		void SetUniforms(gl::Shader& shader, Args&... rest) {
			SetUniformsHelper(shader, 0, rest...);
		}
	}

	template<GLuint(*allocate)(), void(*deallocate)(GLuint)>
	struct Identifier {
		Identifier() {
			id = (*allocate)();
		}

		~Identifier() {
			if (id != 0) {
				deallocate(id);
				id = 0;
			}
		}
		
		operator GLuint() const { return id; }

		void reset() {
			deallocate(id);
			id = (*allocate)();
		}

		GLuint id;
	};

	namespace impl {
		static inline GLuint BufferAllocator() {
			GLuint id;
			glGenBuffers(1, &id);
			return id;
		}

		static inline void BufferDeallocator(GLuint id) {
			glDeleteBuffers(1, &id);
		}

		static inline GLuint VAOAllocator() {
			GLuint id;
			glGenVertexArrays(1, &id);
			return id;
		}

		static inline void VAODeallocator(GLuint id) {
			glDeleteVertexArrays(1, &id);
		}
	}

	typedef typename Identifier<impl::BufferAllocator, impl::BufferDeallocator> BufferIndex;
	typedef typename Identifier<impl::VAOAllocator, impl::VAODeallocator> VAOIndex;

	namespace impl {
		template<typename T>
		inline GLenum toGLenum() {
			return T::enum_type;
		}
		template<>
		inline GLenum toGLenum<float>() {
			return GL_FLOAT;
		}
		template<>
		inline GLenum toGLenum<int>() {
			return GL_INT;
		}
		template<>
		inline GLenum toGLenum<unsigned char>() {
			return GL_UNSIGNED_BYTE;
		}
		template<>
		inline GLenum toGLenum<char>() {
			return GL_BYTE;
		}
		template<>
		inline GLenum toGLenum<double>() {
			return GL_DOUBLE;
		}
		template<>
		inline GLenum toGLenum<unsigned int>() {
			return GL_UNSIGNED_INT;
		}
		template<>
		inline GLenum toGLenum<glm::vec4>() {
			return GL_FLOAT;
		}
		template<>
		inline GLenum toGLenum<glm::vec3>() {
			return GL_FLOAT;
		}
		template<>
		inline GLenum toGLenum<glm::vec2>() {
			return GL_FLOAT;
		}
		template<>
		inline GLenum toGLenum<glm::uvec4>() {
			return GL_UNSIGNED_INT;
		}
		template<>
		inline GLenum toGLenum<glm::uvec3>() {
			return GL_UNSIGNED_INT;
		}
		template<>
		inline GLenum toGLenum<glm::uvec2>() {
			return GL_UNSIGNED_INT;
		}

		template<int index, class _Tuple>
		static inline void AddSingleVertexAttribute(bool normalize, int location) {
			constexpr GLuint size = impl::to_size_v<std::tuple_element_t<index, _Tuple>>;
			constexpr GLenum type = impl::to_glenum_v<std::tuple_element_t<index, _Tuple>>;
			const std::ptrdiff_t offset = impl::tuple_inner_offset<index, _Tuple>();
			glEnableVertexAttribArray(location);
			if constexpr (std::is_integral_v<std::tuple_element_t<index, _Tuple>>) {
				glVertexAttribIPointer(location, size, type, sizeof(_Tuple), reinterpret_cast<void*>(offset));
			}
			else {
				glVertexAttribPointer(location, size, type, normalize, sizeof(_Tuple), reinterpret_cast<void*>(offset));
			}
		}

		template<int index, class _Tuple>
		static void AddVertexAttribute(bool normalize, int indexOffset) {
			if constexpr (index < std::tuple_size_v<_Tuple>) {
				const int idx = index + indexOffset;
				AddSingleVertexAttribute<index, _Tuple>(normalize, idx);
				AddVertexAttribute<index + 1, _Tuple>(normalize, indexOffset);
			}
		}


		template<typename T, typename... Types>
		static constexpr bool is_any_v = std::disjunction_v<std::is_same<std::remove_cv_t<T>, Types>...>;

		template<typename T, typename... Types>
		struct is_any : std::bool_constant<is_any_v<T, Types...>> {};

		template<typename T, typename... Types>
		static constexpr bool derives_any_v = std::disjunction_v<std::is_base_of<Types, std::remove_cv_t<T>>...>;

		template<typename T, typename... Types>
		struct derives_any : std::bool_constant<derives_any_v<T, Types...>> {};

		template <typename... Types>
		static constexpr bool static_check_types_v =
			std::disjunction_v<is_any<Types,
			glm::vec4, glm::vec3, glm::vec2,
			glm::uvec4, glm::uvec3, glm::uvec2,
			//Eigen::Vector4f, Eigen::Vector3f,
			unsigned int, int, float>...>;

		template<typename T, bool _Condition, class _First_integral, class... _Traits>
		struct _Select_Value {
			static constexpr T value = _First_integral::value;
		};

		template<typename T, class _First_integral, class... _Traits>
		struct _Select_Value<T, false, _First_integral, _Traits...> {
			static constexpr GLenum value = select_value_v<T, _Traits...>;
		};

		template<typename T, class _First_condition, class... _Traits>
		static constexpr auto select_value_v = _Select_Value<T, _First_condition::value, _Traits...>::value;

		template<typename T, class _First, class... _Traits>
		struct select_value : std::integral_constant<T, select_value_v<T, _First, _Traits>> {};

		template<typename T>
		static constexpr GLenum to_glenum_v = select_value_v<GLenum,
			is_any<T, glm::vec4, glm::vec3, glm::vec2, /*Eigen::Vector4f, Eigen::Vector3f, Eigen::Vector2f,*/ float>, std::integral_constant<GLenum, GL_FLOAT>,
			is_any<T, glm::uvec4, glm::uvec3, glm::uvec3, unsigned int, ImGuiID>, std::integral_constant<GLenum, GL_UNSIGNED_INT>,
			std::bool_constant<true>, std::integral_constant<GLenum, GL_INT>
		>;

		template<typename T>
		static constexpr GLuint to_size_v = select_value_v<GLuint,
			is_any<T, glm::uvec4, glm::vec4/*, Eigen::Vector4f*/>, std::integral_constant<GLuint, 4>,
			is_any<T, glm::uvec3, glm::vec3/*, Eigen::Vector3f*/>, std::integral_constant<GLuint, 3>,
			is_any<T, glm::uvec2, glm::vec2/*, Eigen::Vector2f*/>, std::integral_constant<GLuint, 2>,
			std::bool_constant<true>, std::integral_constant<GLuint, 1>
		>;

		template<int N, class _Tuple>
		static std::ptrdiff_t tuple_inner_offset() {
			_Tuple val;
			return reinterpret_cast<char*>(&std::get<N>(val)) - reinterpret_cast<char*>(&val);
		}

		template<typename T>
		static constexpr bool is_floating_point_v = 
			is_any_v<T, glm::vec4, glm::vec3, glm::vec2, /*Eigen::Vector4f, Eigen::Vector3f, Eigen::Vector2f,*/ float>;

		template<typename T>
		static constexpr size_t get_dimension_v = select_value_v<size_t,
			is_any<T, glm::vec4/*, Eigen::Vector4f*/>,	std::integral_constant<std::size_t, 4>,
			is_any<T, glm::vec3/*, Eigen::Vector3f*/>,	std::integral_constant<std::size_t, 3>,
			is_any<T, glm::vec2/*, Eigen::Vector2f*/>,	std::integral_constant<std::size_t, 2>,
			is_any<T, glm::vec<1, float, glm::defaultp>>, std::integral_constant<std::size_t, 1>>;


	}

	static void APIENTRY openglCallbackFunction(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam) {

		// Some filters
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

		std::cout << "---------------------opengl-Error------------" << std::endl;
		std::cout << "message: " << message << std::endl;
		std::cout << "type: ";
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
		}
		std::cout << std::endl;

		std::cout << "id: " << id << std::endl;
		std::cout << "severity: ";
		switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cout << "NOTIFICATION";
			break;
		default:
			std::cout << "UNKNOWN";
		}
		std::cout << std::endl;
		std::cout << "---------------------opengl-Error-end--------------" << std::endl;
#ifdef _MSC_VER 
		if (severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR)
			__debugbreak();
#endif
	}
}