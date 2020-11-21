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
	float k_ambi = 0.25f;
	float k_diff = 0.75f;
	float k_spec = 0.20f;
	float n = 30.0f;
	vec3 lightpos = vec3(0, 0, 5);

	vec3 L = normalize(lightpos - pos);
	vec3 E = normalize(-pos);
	vec3 R = normalize(-reflect(L, N)); 
	
	vec4 Iambi = color;
	vec4 Idiff = color * max(dot(N, L), 0.0);
	vec4 Ispec = vec4(1, 1, 1, 1) * pow(max(dot(R, E), 0.0), 0.3*n);

	FragColor = k_ambi * Iambi + k_diff * Idiff + k_spec * Ispec;
}