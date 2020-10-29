#include "intermediate.h"

const std::string vertex_display_shader_up = R"(
#version 430
out vec2 texCoord;

void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    texCoord.x = (x+1.0)*0.5;
    texCoord.y = (y+1.0)*0.5;
	gl_Position = vec4(x, y, 0, 1);
}
)";

const std::string vertex_display_shader_down = R"(
#version 430
out vec2 texCoord;

void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    texCoord.x = (x+1.0)*0.5;
    texCoord.y = 1.0 - (y+1.0)*0.5;
	gl_Position = vec4(x, y, 0, 1);
}
)";

const std::string fragment_display_shader = R"(
#version 430
in vec2 texCoord;

uniform sampler2D image;
layout(location = 0) out vec4 FragColor;


void main()
{
	FragColor = texture(image, texCoord);
}
)";

std::shared_ptr<gl::Shader> gl::textureDispShader(bool uvUp)
{
    static std::shared_ptr<gl::Shader> up_shader = nullptr;
    static std::shared_ptr<gl::Shader> down_shader = nullptr;

    if (uvUp) {
        if (up_shader == nullptr) {
            up_shader = std::make_shared<gl::Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
                { GL_VERTEX_SHADER, vertex_display_shader_up },
                { GL_FRAGMENT_SHADER, fragment_display_shader }
            });
        }
        return up_shader;
    }
    else {
        if (down_shader == nullptr) {
            down_shader = std::make_shared<gl::Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
                { GL_VERTEX_SHADER, vertex_display_shader_down },
                { GL_FRAGMENT_SHADER, fragment_display_shader }
            });
        }
        return down_shader;
    }
}

void gl::displayTexture(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, bool uvUp)
{
    fullscreenTriangle(x, y, width, height, gl::textureDispShader(uvUp),
        "image", tex);
}

void gl::displayTexture(int x, int y, int width, int height, gl::Texture& tex, bool uvUp)
{
    fullscreenTriangle(x, y, width, height, gl::textureDispShader(uvUp),
        "image", tex);
}
