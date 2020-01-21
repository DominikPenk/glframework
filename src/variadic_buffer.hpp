#pragma once

#include <tuple>

#include <glm/glm.hpp>

#include <eigen3/Eigen/Core>

#include "imgui.h"

#include "gl_internal.hpp"

namespace gl {

	template<typename ...Args>
	class CompactVertexBufferObject : public gl::VertexBufferObjectBase {
	public:
		typedef typename std::tuple<Args ...> value_type;

		typedef typename std::vector<value_type>::iterator iterator;
		typedef typename std::vector<value_type>::const_iterator const_iterator;
		
		CompactVertexBufferObject() :
			VertexBufferObjectBase(sizeof(value_type))
		{
			// We only allow certain types here (I know its ugly)
			static_assert(impl::static_check_types_v<Args...>, "Invalid template argument!");
		}

		void resize(std::size_t size) {
			mData.resize(size);
		}
		
		void push_back(const Args&... data) {
			mData.push_back(std::make_tuple(data...));
			mUpdated = true;
		}

		void extend(std::initializer_list<value_type> data) {
			mData.insert(mData.end(), data);
			mUpdated = true;
		}

		inline iterator begin() { return mData.begin(); }
		inline const_iterator begin() const { return mData.begin(); }

		inline iterator end() { return mData.end(); }
		inline const_iterator end() const { return mData.end(); }

		template<int i = -1>
		auto get(int idx) const {
			if constexpr (i < 0) 
				return mData[idx];
			else 
				return std::get<i>(mData[idx]);
		}

		template<int i = -1>
		auto& get(int idx) {
			if constexpr (i < 0)
				return mData[idx];
			else
				return std::get<i>(mData[idx]);
		}

		virtual size_t size() const override { return mData.size(); }
		
		value_type operator[](unsigned int i) const {
			return mData[i];
		}

		value_type& operator[](unsigned int i) {
			return mData[i];
		}
	
		void clear() {
			mData.clear();
			mUpdated = true;
		}

	private:
		virtual const void* dataPtr() const {
			return reinterpret_cast<const void*>(mData.data());
		}

	protected:
		std::vector<value_type> mData;

	};

}