#version 430 core
// --vertex
layout (location = 0) in vec3 position;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(position, 1.0);
}

// --fragment

uniform vec4 color;

out vec4 FragColor;

void main() {
	FragColor = color;
}