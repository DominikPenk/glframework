#pragma once
/**
* You should include this header if you are using imgui through the glpp framework
* It conveniently imports additional Icons using Awesomefont and the extensions wrapping glpp classes for use with imgui
*/

// Import of the acutal imgui interface
// Make sure that vector operations are defined
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "glpp/IconsFontAwesome5.h"
#include "glpp/imgui_ext.hpp"
#include "glpp/imgui_vertical_tabs.hpp"
#include "glpp/imgui_filesystem.hpp"
#include "glpp/imgui_statistics.hpp"

#include <glm/glm.hpp>

typedef typename glm::mat2 ImMat2;


template<typename T>
inline ImVec2 glm2ImGui(glm::vec<2, T> v) {
	return ImVec2(v.x, v.y);
}

template<typename T>
inline ImVec4 glm2ImGui(glm::vec<4, T> v) {
	return ImVec4(v.x, v.y, v.z, v.w);
}

inline glm::vec2 imGui2Glm(ImVec2 v) {
	return glm::vec2(v.x, v.y);
}

inline glm::vec4 imgui2ImGlm(ImVec4 v) {
	return glm::vec4(v.x, v.y, v.z, v.w);
}

inline ImVec2 operator*(const glm::mat2& A, const ImVec2 v) {
	return ImVec2(
		A[0][0] * v.x + A[1][0] * v.y, 
		A[0][1] * v.x + A[1][1] * v.y);
}