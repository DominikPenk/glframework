#pragma once

static const char* TRIANGLE_NORMAL_VS = R"(
#version 330
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 vNormal;

out vec3 normal;

uniform mat4 MVP;
uniform mat4 M;


void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
	normal = (M * vec4(vNormal, 0.0)).xyz;
}
)";


static const char* TRIANGLE_NORMAL_FS = R"(
#version 330
in vec3 normal;

out vec4 FragColor;

void main() {
	FragColor = vec4(abs(normalize(normal)), 1.0f);
}
)";