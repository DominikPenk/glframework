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