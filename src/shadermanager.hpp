#pragma once
#include <glad/glad.h>

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <filesystem>

#include "buffers.hpp"


namespace gl {
	struct Layout {
		GLint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLint stride;
		const GLvoid *start;
		Layout(GLint index, GLint size, GLenum type, GLboolean normalized = GL_FALSE, GLint stride = 0, const GLvoid *start = (void*)0) :
			index(index),
			size(size),
			type(type),
			normalized(normalized),
			stride(stride),
			start(start) {}
	};

	struct ShaderRequirements {
		std::vector<GLenum> requirements;
		std::vector<Layout> vertexAttributes;
		ShaderRequirements(const std::vector<GLenum>& requirements,
			const std::vector<Layout>& vattrs) : requirements(requirements), vertexAttributes(vattrs) {
			for (GLenum requirement : requirements) {
				glEnable(requirement);
			}
			for (auto attribute : vertexAttributes) {
				glEnableVertexAttribArray(attribute.index);
				glVertexAttribPointer(
					attribute.index,
					attribute.size,
					attribute.type,
					attribute.normalized,
					attribute.stride,
					attribute.start);
			}
		}

		~ShaderRequirements() {
			for (GLenum requirement : requirements) {
				glDisable(requirement);
			}
			for (auto attribute : vertexAttributes) {
				glDisableVertexAttribArray(attribute.index);
			}
		}

	};

	class Shader {
	public:
		Shader();

		Shader(std::string path);

		~Shader();

		void requires(GLenum cap) {
			if (std::find(mEnables.begin(), mEnables.end(), cap) != mEnables.end()) {
				std::cerr << "Double requested requirement" << std::endl;
				return;
			}
			mEnables.push_back(cap);
		}

		void VertexAttribute(int index, int size, GLenum type) {
			mVertexAttributes.push_back(Layout(index, size, type, false, 0, (void*)0));
		}
		void VertexAttribute(Layout& layout) {
			mVertexAttributes.push_back(layout);
		}

		ShaderRequirements require() const { return ShaderRequirements(mEnables, mVertexAttributes); }

		/// Return the program id
		GLuint program() const { return mProgram; }

		/// Call this function to update the Shader by reloading the sources
		void update();

		void setDefine(const std::string& name, const std::string& value);
		void setDefine(const std::string& name, int value);
		void setDefine(const std::string& name, float value);
		void setDefineFlag(const std::string& name);
		void removeDefine(const std::string& name);
		bool hasDefine(const std::string& name);

		/// Uniform setters
		inline void setUniform(const std::string& name, float value) const {
			glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value);
		}
		inline void setUniform(const std::string& name, const glm::vec2& v) const {
			glUniform2f(glGetUniformLocation(mProgram, name.c_str()), v.x, v.y);
		}
		inline void setUniform(const std::string& name, const glm::vec3& v) const {
			glUniform3f(glGetUniformLocation(mProgram, name.c_str()), v.x, v.y, v.z);
		}
		inline void setUniform(const std::string& name, const glm::vec4& v) const {
			glUniform4f(glGetUniformLocation(mProgram, name.c_str()), v.x, v.y, v.z, v.w);
		}
		inline void setUniform(const std::string& name, int value) const {
			glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
		}
		inline void setUniform(const std::string& name, unsigned int value) const {
			glUniform1ui(glGetUniformLocation(mProgram, name.c_str()), value);
		}

		template<typename T, int n>
		inline void setShaderStorageBuffer(VertexBufferObject<T, n>& buffer, GLuint index) const {
			buffer.bind(GL_SHADER_STORAGE_BUFFER);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer.id());
			buffer.unbind(GL_SHADER_STORAGE_BUFFER);
		}

		void setUniform(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setUniform(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		ShaderRequirements use();

		GLuint textureId(std::string name) {
			int id = glGetUniformLocation(mProgram, name.c_str());
			if(id == -1)
				std::cerr << "Uniform " << name << " not found in shader" << std::endl;
			return (GLuint)id;
		}

	protected:
		bool requiresUpdate() const;

		GLuint mProgram;
		std::vector<std::string> mSourceFiles;
		std::vector<GLenum> mEnables;
		std::vector<Layout> mVertexAttributes;
		std::unordered_map<std::string, std::string> mDefines;
		long long mLastUpdated;
	};
}