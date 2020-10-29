#version 330

// --vertex
layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;

out vec3 normal;

uniform mat4 MVP;
uniform mat4 M;


void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
	normal = (M * vec4(vNormal, 0.0)).xyz;
}

// --fragment
in vec3 normal;

out vec4 FragColor;

void main() {
	FragColor = vec4(abs(normalize(normal)), 1.0f);
}