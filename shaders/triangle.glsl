#version 330

// --vertex
layout(location = 0) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;

uniform mat4 MVP;
uniform mat4 M;

out vec3 N;
out vec3 pos;

void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
	pos = gl_Position.xyz;
	N = normalize(MVP * vec4(vNormal, 0.0)).xyz; 
}

// --fragment
uniform vec4 color;
in vec3 N;
in vec3 pos;

out vec4 FragColor;


void main() {
	vec3 L = normalize(vec3(0, 0, 5) - pos);
	vec4 Idiff = color * max(dot(N, L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);
	FragColor = Idiff;
}