#version 430

// --vertex
out vec2 texCoord;

uniform vec4 tl;
uniform vec2 size;
uniform vec2 uvmin;
uniform vec2 uvmax;

void main()
{
    vec2 uv =  vec2(
			float(((uint(gl_VertexID) + 2u) / 3u)%2u),
			float(((uint(gl_VertexID) + 1u) / 3u)%2u));
    texCoord = mix(uvmin, uvmax, uv);
	gl_Position = vec4(tl.xy + uv * size, tl.z, 1);
}


// --fragment
in vec2 texCoord;
in vec2 pos;

uniform sampler2D image;
uniform vec4	  color;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = color * texture(image, texCoord);
}