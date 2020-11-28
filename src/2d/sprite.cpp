#include "glpp/2d/sprite.hpp"

#include "glpp/2d/canvas.hpp"
#include "glpp/texture.hpp"
#include "glpp/shadermanager.hpp"

#pragma region Shaders
const std::string sprite_vertex_shader = R"(
#version 430
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
	gl_Position = vec4(tl.xy + uv * size, 0, 1);
}
)";

const std::string sprite_fragment_shader = R"(
#version 430
in vec2 texCoord;
in vec2 pos;

uniform sampler2D image;
uniform vec4	  color;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = color * texture(image, texCoord);
}
)";

#pragma endregion

gl::Sprite::Sprite() :
	texture(defaultSpriteTexture()),
	shader(defaultSpriteShader()),
	color(1),
	position(0, 0),
	size(-1, -1),
	layer(0),
	minUV(0),
	maxUV(1)
{
}

gl::Sprite::Sprite(std::string image, bool flipY) :
	Sprite()
{
	setTexture(image, flipY);
}

void gl::Sprite::render(int width, int height, int layers)
{
	// Compute position
	const glm::vec2 canvasSize(width, height);
	const glm::vec3 tl(
		-1.0f + 2.0f * position / canvasSize,
		 1.0f - 2.0f * std::clamp(layer, 1, layers - 1) / layers);
	const glm::vec2 relSize = size / canvasSize;
	
	// Enable depth test
	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

	auto _ = shader->use();
	shader->setUniforms(
		"tl", glm::vec4(tl, 1.0f),
		"uvmin", minUV,
		"uvmax", maxUV,
		"size", relSize,
		"image", texture,
		"color", glm::vec4(color, 1.0f)
	);

	glEnable(GL_DEPTH_TEST);
	gl::VertexArrayObject::bindDummy();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if (depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
}

void gl::Sprite::setTexture(std::shared_ptr<gl::Texture> texture)
{
	this->texture = texture;
	size = glm::vec2(texture->cols, texture->rows);
}

std::shared_ptr<gl::Texture> gl::Sprite::setTexture(std::string path, bool flipY)
{
	texture = std::make_shared<gl::Texture>(path, flipY);
	size = glm::vec2(texture->cols, texture->rows);
	if (!flipY) {
		minUV = glm::vec2(0, 1);
		maxUV = glm::vec2(1, 0);
	}
	return texture;
}

std::shared_ptr<gl::Shader> gl::defaultSpriteShader()
{
	static std::shared_ptr<gl::Shader> shader = nullptr;
	
	if (shader == nullptr) {
		//shader = std::make_shared<gl::Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
		//	{ GL_VERTEX_SHADER, sprite_vertex_shader },
		//	{ GL_FRAGMENT_SHADER, sprite_fragment_shader }
		//});
		shader = std::make_shared<gl::Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "sprite.glsl");
	}
	return shader;
}

std::shared_ptr<gl::Texture> gl::defaultSpriteTexture()
{
	static std::shared_ptr<gl::Texture> tex = nullptr;
	if (tex == nullptr) {
		unsigned char texData[] = { 255, 255, 255, 255 };
		tex = std::make_shared<gl::Texture>(1, 1);
		tex->setData(texData, GL_RGBA);
	}
	return tex;
}
