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
	gl_Position = vec4(tl.xy + uv * size, tl.z, 1);
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
	CanvasElement(),
	texture(defaultSpriteTexture()),
	shader(defaultSpriteShader()),
	color(1),
	position(0, 0),
	size(-1, -1),
	minUV(0),
	maxUV(1)
{
	hasInteractions = false;
}

gl::Sprite::Sprite(std::string image, bool flipY) :
	Sprite()
{
	setTexture(image, flipY);
}

void gl::Sprite::draw(int width, int height, int layers)
{
	// Compute position
	const glm::vec2 canvasSize(width, height);
	const glm::vec3 bl(
		-1.0f + 2.0f * position / canvasSize,
		 1.0f - 2.0f * std::clamp(layer, 1, layers - 1) / layers);
	const glm::vec2 relSize = 2.0f * size / canvasSize;
	
	

	auto _ = shader->use();
	shader->setUniforms(
		"bl", glm::vec4(bl, 1.0f),
		"uvmin", minUV,
		"uvmax", maxUV,
		"size", relSize,
		"image", texture,
		"color", glm::vec4(color, 1.0f)
	);

	gl::VertexArrayObject::bindDummy();
	glDrawArrays(GL_TRIANGLES, 0, 6);

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

bool gl::Sprite::overlaps(int x, int y) const
{
	return (x >= position.x && x < position.x + size.x) && (y >= position.y && y < position.y + size.y);
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
