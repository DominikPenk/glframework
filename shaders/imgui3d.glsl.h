#pragma once

static const char* IMGUI3D_VS = R"(#version 430 core
// --vertex
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in uint id;
layout (location = 3) in vec2 uv;

uniform mat4 VP;

out vec4 fcol;
out vec4 idcol;
out vec2 fuv;

vec4 id2col(uint id) {
	return vec4(float(id % 256) / 255, float((id / 256) % 256) / 255, float(id / (256*256)) / 255, 1.0);
}

void main() {
	gl_Position = VP * position;
    fcol = color;
	idcol = id2col(id);
	fuv = uv;
}
)";

static const char* IMGUI3D_FS = R"(
#version 430 core
in vec4 fcol;
in vec4 idcol;
in vec2 fuv;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 IdColor;

void main() {
	FragColor = fcol;
	IdColor = idcol;
})";