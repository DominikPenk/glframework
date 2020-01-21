#version 330

// --vertex
layout(location = 0) in vec3 vPosition;

out vec3 fColor;

uniform mat4 MVP;
uniform vec4 color;

void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
	fColor = vec3(color);
}

// --fragment
in vec3 fColor;

out vec4 FragColor;

void main() {
	FragColor = vec4(fColor, 1.0);
}