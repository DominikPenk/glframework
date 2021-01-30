#version 330

// --vertex
layout(location = 0) in vec3 vPosition;

out vec3 vColor;
uniform mat4 MVP;
uniform vec4 color;

void main() {
	gl_Position = MVP * vec4(vPosition, 1.0);
    vColor = vec3(color);
}


// --fragment
in vec3 fColor;
flat in uvec3 vids;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint FaceId;

void main() {
	FragColor = mix(
        vec4(1.0, 1.0, 1.0, 1.0),
        vec4(1.0, 0.0, 0.0, 1.0),
        gl_PrimitiveID / 2463.0);
    FaceId = uint(gl_PrimitiveID);
}