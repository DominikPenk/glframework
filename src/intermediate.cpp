#include "intermediate.h"

const std::string vertex_display_shader = R"(
#version 430
out vec2 texCoord;

uniform vec2 uvmin;
uniform vec2 uvmax;

void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    texCoord = mix(uvmin, uvmax, vec2((x+1.0)*0.5, (y+1.0)*0.5));
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

const std::string fragment_scalar_display_shader = R"(
#version 430
in vec2 texCoord;

uniform sampler2D scalars;
uniform float vmin;
uniform float vmax;
uniform vec4 cmin;
uniform vec4 cmax;

layout(location = 0) out vec4 FragColor;

float linearstep(float v, float vmin, float vmax) {
    return min((v - vmin) / (vmax - vmin), 1.0);
}

void main()
{
	float t = linearstep(texture(scalars, texCoord).r, vmin, vmax);
    if(t < 0) {
        FragColor = vec4(0, 0, 0, 1);
    } else {
        FragColor = mix(cmin, cmax, t);
    }
}
)";

std::shared_ptr<gl::Shader> gl::textureDispShader()
{
    static std::shared_ptr<gl::Shader> shader = nullptr;

    if (shader == nullptr) {
        shader = std::make_shared<gl::Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
            { GL_VERTEX_SHADER, vertex_display_shader },
            { GL_FRAGMENT_SHADER, fragment_display_shader }
        });
    }
    return shader;
}

std::shared_ptr<gl::Shader> gl::scalarDispShader()
{
    static std::shared_ptr<gl::Shader> shader = nullptr;

    if (shader == nullptr) {
        shader = std::make_shared<gl::Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
            { GL_VERTEX_SHADER, vertex_display_shader },
            { GL_FRAGMENT_SHADER, fragment_scalar_display_shader }
        });
    }
    return shader;
}

void gl::displayTexture(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, glm::vec2 uvmin, glm::vec2 uvmax)
{
    fullscreenTriangle(x, y, width, height, gl::textureDispShader(),
        "image", tex,
        "uvmin", uvmin,
        "uvmax", uvmax);
}

void gl::displayTexture(int x, int y, int width, int height, gl::Texture& tex, glm::vec2 uvmin, glm::vec2 uvmax)
{
    fullscreenTriangle(x, y, width, height, gl::textureDispShader(),
        "image", tex,
        "uvmin", uvmin,
        "uvmax", uvmax);
}

void gl::displayScalars(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, glm::vec2 range, glm::vec4 minColor, glm::vec4 maxColor, glm::vec2 uvmin, glm::vec2 uvmax)
{
    fullscreenTriangle(x, y, width, height, gl::scalarDispShader(),
        "scalars", tex,
        "uvmin", uvmin,
        "uvmax", uvmax,
        "vmin", range.x,
        "vmax", range.y,
        "cmin", minColor,
        "cmax", maxColor);
}

void gl::displayScalars(int x, int y, int width, int height, gl::Texture& tex, glm::vec2 range, glm::vec4 minColor, glm::vec4 maxColor, glm::vec2 uvmin, glm::vec2 uvmax)
{
    fullscreenTriangle(x, y, width, height, gl::scalarDispShader(),
        "scalars", tex,
        "uvmin", uvmin,
        "uvmax", uvmax,
        "vmin", range.x,
        "vmax", range.y,
        "cmin", minColor,
        "cmax", maxColor);
}

