#include "shadermanager.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <streambuf>
#include <filesystem>
#include <regex>
#include <map>

using namespace gl;

#pragma region implementation details
constexpr GLenum ShaderPipeline[] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

static std::string variablePrefix(GLenum shader) {
	if (shader == GL_VERTEX_SHADER) {
		return "v";
	}
	else if (shader == GL_TESS_CONTROL_SHADER) {
		return "tc";
	}
	else if (shader == GL_TESS_EVALUATION_SHADER) {
		return "te";
	}
	else if (shader == GL_GEOMETRY_SHADER) {
		return "ge";
	}
	else if (shader == GL_FRAGMENT_SHADER) {
		return "f";
	}
	else {
		throw std::invalid_argument("Unknown shader type");
	}
}

struct Prefix {
	std::string code;
	std::vector<std::pair<std::string, std::string>> passings;
	std::vector<std::pair<std::string, std::string>> defines;

	std::string prefix_code(GLenum shader, GLenum previousShader) const {
		std::stringstream src;
		src << code << std::endl;
		if (defines.size() > 0) {
			src << "// Defines Added by compiler" << std::endl;
			for (const auto& define : defines) {
				src << "#define " << define.first << " " << define.second << std::endl;
			}
			src << "// -> End defines " << std::endl; 
		}
		if (passings.size() > 0) {
			src << "// Auto generated variables" << std::endl;
			std::string inPrefix = (previousShader != 0) ? variablePrefix(previousShader) : "";
			std::string outPrefix = variablePrefix(shader);
			for (const auto& p : passings) {
				if (shader == GL_TESS_CONTROL_SHADER || shader == GL_TESS_EVALUATION_SHADER || shader == GL_GEOMETRY_SHADER) {
					src << "in " << p.first << " " << inPrefix << p.second << "[];" << std::endl;
				}
				else if (shader != GL_VERTEX_SHADER) {
					src << "in " << p.first << " " << inPrefix << p.second << ";" << std::endl;
				}
				if (shader == GL_TESS_CONTROL_SHADER) {
					src << "out " << p.first << " " << outPrefix << p.second << "[];" << std::endl;
				}
				else {
					src << "out " << p.first << " " << outPrefix << p.second << ";" << std::endl;
				}
			}
			src << "// -> End auto generated variables" << std::endl << std::endl;
		}
		return src.str();
	}

	std::string replace_passings(std::string code, GLenum shader, GLenum previous) const {
		if (passings.size() == 0) 
			return code;
		std::string in_regex_str = "in?(";
		std::string out_regex_str = "o(?:ut)?(";
		for (int i = 0; i < (int)passings.size(); ++i) {
			in_regex_str += "(?:" + passings[i].second + ")";
			out_regex_str += "(?:" + passings[i].second + ")";
			if (i < (int)passings.size() - 1) {
				in_regex_str += "|";
				out_regex_str += "|";
			}
		}
		in_regex_str += ")";
		out_regex_str += ")";
		if (shader != GL_VERTEX_SHADER)
			code = std::regex_replace(code, std::regex(in_regex_str), variablePrefix(previous) + "$1");
		if (shader != GL_FRAGMENT_SHADER)
			code = std::regex_replace(code, std::regex(out_regex_str), variablePrefix(shader) + "$1");
		return code;
	}
};

struct ShaderCode {
public:
	std::vector<std::string> code;
	std::vector<int> startLine;

	std::vector<std::string> getPrefixedCode() const {
		std::vector<std::string> codes;
		for (int i = 0; i < (int)code.size(); ++i) {
			codes.push_back("#line " + std::to_string(startLine[i]) + "\n" + code[i]);
		}
		return codes;
	}

	std::string getCode() const {
		std::string result = "";
		for (int i = 0; i < (int)code.size(); ++i) {
			result += "#line " + std::to_string(startLine[i]) + "\n" + code[i];
		}
		return result;
	}
};

std::string resolveRelative(std::string relPath, std::string workingDir = std::filesystem::current_path().string()) {
	std::filesystem::path oldWd = std::filesystem::current_path();
	std::filesystem::current_path(workingDir);

	if (!relPath[0] != '.') {
		relPath = "./" + relPath;
	}
	std::string result = std::filesystem::weakly_canonical(relPath).string();

	std::filesystem::current_path(oldWd);

	return result;
}

bool validatePipeline(const std::map<GLenum, ShaderCode>& pipeline) {
	bool valid = true;
	if (pipeline.find(GL_VERTEX_SHADER) == pipeline.end()) {
		std::cerr << "Missing shader: Vertex Shader" << std::endl;
		valid = false;
	}
	if (pipeline.find(GL_FRAGMENT_SHADER) == pipeline.end()) {
		std::cerr << "Missing shader: Fragment Shader" << std::endl;
		valid = false;
	}
	if (pipeline.find(GL_TESS_CONTROL_SHADER) != pipeline.end()
		&& pipeline.find(GL_TESS_EVALUATION_SHADER) == pipeline.end()) {
		std::cerr << "Missing shader: Tesselation Evaluation Shader (but Tesselation Control Shader was found)" << std::endl;
		valid = false;
	}
	if (pipeline.find(GL_TESS_EVALUATION_SHADER) != pipeline.end()
		&& pipeline.find(GL_TESS_CONTROL_SHADER) == pipeline.end()) {
		std::cerr << "Missing shader: Tesselation Control Shader (but Tesselation Evaluation Shader was found)" << std::endl;
		valid = false;
	}
	return valid;
}

std::vector<std::string> toLines(const std::string& string) {
	std::vector<std::string> result;
	std::string temp;
	int markbegin = 0;
	int markend = 0;

	for (int i = 0; i < string.length(); ++i) {
		if (string[i] == '\n') {
			markend = i;
			result.push_back(string.substr(markbegin, markend - markbegin));
			markbegin = (i + 1);
		}
	}
	return result;
}

std::string trim(std::string str, bool trimNewlines) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [=](char c) {
		return std::isspace(c) || (trimNewlines && c == '\n') || (trimNewlines && c == '\r');
	}));
	str.erase(std::find_if(str.rbegin(), str.rend(), [=](char c) {
		return std::isspace(c) || (trimNewlines && c == '\n') || (trimNewlines && c == '\r');
		}).base(), str.end());
	return str;
}

Prefix parsePrefix(const std::string& code) {
	std::regex passing_regex(R"(\s*pass\s+(\w+)\s+(\w+)\s*;\s*[\n\r]*)");

	std::vector<std::string> lines = toLines(code);
	lines.insert(lines.begin() + 1, "#line 2");

	Prefix p;
	std::stringstream preamble_code;
	std::smatch m;
	for (const std::string& line : lines) {
		if (!std::regex_match(line, m, passing_regex)) {
			preamble_code << line << std::endl;
		}
		else {
			std::string type = m[1].str();
			std::string name = m[2].str();
			std::cout << "Found passing: " << name << " (Type: " << type << ")" << std::endl;
			p.passings.push_back(std::make_pair(type, name));
		} 
	}
	p.code = preamble_code.str();
	return p;
}

std::pair<bool, GLuint> loadAndCompileShader(const ShaderCode& src, GLenum type, GLenum previous, const Prefix& prefix) {
	std::string prefix_code = prefix.prefix_code(type, previous);
	
	std::vector<std::string> sources = src.getPrefixedCode();
	std::transform(sources.begin(), sources.end(), sources.begin(), [&](const std::string& code) {
		return prefix.replace_passings(code, type, previous);
	});
	sources.insert(sources.begin(), prefix_code);
	std::vector<int> lengths(sources.size());
	std::vector<const char*> src_ptr(sources.size());
	std::transform(sources.begin(), sources.end(), lengths.begin(), [](const std::string& code) {
		return code.size();
	});
	std::transform(sources.begin(), sources.end(), src_ptr.begin(), [](const std::string& code) {
		return code.c_str();
	});
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, src_ptr.data(), lengths.data());
	glCompileShader(shader);
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
		std::cerr << "Shader compilation failed:" << std::endl;
		std::string compilationLog;
		compilationLog.resize(512);
		glGetShaderInfoLog(shader, compilationLog.size(), NULL, compilationLog.data());

		// Print 
		std::vector<std::string> lines = toLines(prefix_code + src.getCode());
		for (size_t i = 0; i < lines.size(); ++i) {
			std::cerr << std::left << std::setw(6) << 1+i << lines[i] << std::endl;
		}

		// Assume the first integer is the src id
		std::regex fid_re(R"((\d+))");
		std::smatch match;
		std::regex_replace(compilationLog, fid_re, "FileId: $1");

		std::cout << std::endl << "----------------------------------" << std::endl;
		std::cerr << compilationLog << std::endl;
		return std::make_pair(false, (GLuint)0);
	}
	return std::make_pair(true, shader);
}

static std::pair<bool, GLuint> compileShader(std::string src, GLenum type) {
	GLuint shader = glCreateShader(type);
	const GLchar* src_ptr = (const GLchar*)src.c_str();
	glShaderSource(shader, 1, &src_ptr, NULL);
	glCompileShader(shader);
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
		std::cerr << "Shader compilation failed:" << std::endl;
		std::string compilationLog;
		compilationLog.resize(512);
		glGetShaderInfoLog(shader, compilationLog.size(), NULL, compilationLog.data());

		// Assume the first integer is the src id
		std::regex fid_re(R"((\d+))");
		std::smatch match;
		std::regex_replace(compilationLog, fid_re, "FileId: $1");

		std::cout << std::endl << "----------------------------------" << std::endl;
		std::cerr << compilationLog << std::endl;
		return std::make_pair(false, (GLuint)0);
	}
	return std::make_pair(true, shader);
}

std::tuple<Prefix, std::map<GLenum, ShaderCode>> parseFile(std::ifstream& in, const std::string & srcDir, 
	std::vector<std::string>& sources, const std::unordered_map<std::string, std::string>& _defines = std::unordered_map<std::string, std::string>()) {
	assert(in.is_open(), "Tried to parse closed file");

	constexpr GLenum PREFIX = 0;
	constexpr GLenum REQUIRE_SHADER = -1;
	GLenum currentShaderType = PREFIX;
	std::map<GLenum, ShaderCode> pipeline;
	Prefix prefix;

	std::vector<std::pair<std::string, std::string>> defines;
	if (_defines.size() > 0) {
		defines.resize(_defines.size());
		std::copy(_defines.begin(), _defines.end(), defines.begin());
		prefix.defines = defines;
	}

	// Some regexes we need
	std::regex shader_regex(R"(\/\/\s*--(\w+)\s*)");
	std::regex include_regex(R"(#include\s*(?:\"|<)([\w._-]+)(?:\"|>)(?:\s|;)*)");
	std::regex block_comment_start(R"(\/\*)");
	std::regex block_comment_end(R"(\*\/)");
	std::regex one_line_block_comment(R"(\/\*[^(?:\\\*)]*\*\/)");
	std::regex line_comment(R"(\/\/)");

	bool blockComment = false;
	
	// iterate over file line by line
	std::stringstream shaderSource;
	int lineNumber = 1;
	for (std::string line; std::getline(in, line); ++lineNumber) {
		// check if this line starts a new shader
		std::smatch shader_match;
		if (std::regex_match(line, shader_match, shader_regex)) {
			// Update code
			if (currentShaderType == PREFIX) {
				prefix = parsePrefix(shaderSource.str());
				prefix.defines = defines;
			}
			else {
				pipeline[currentShaderType].code.back() = shaderSource.str();
			}

			// Start the new shader
			if (shader_match[1] == "vertex") {
				currentShaderType = GL_VERTEX_SHADER;
			}
			else if (shader_match[1].str() == "fragment") {
				currentShaderType = GL_FRAGMENT_SHADER;
			}
			else if (shader_match[1].str() == "tesscontrol") {
				currentShaderType = GL_TESS_CONTROL_SHADER;
			}
			else if (shader_match[1].str() == "tesseval") {
				currentShaderType = GL_TESS_EVALUATION_SHADER;
			}
			else if (shader_match[1].str() == "geometry") {
				currentShaderType = GL_GEOMETRY_SHADER;
			}
			else {
				currentShaderType = 0;
				std::cerr << "Unknown shader type: \"" << shader_match[1] << "\"" << std::endl;
			}
			pipeline[currentShaderType] = { { "" }, { lineNumber + 1 } };
			shaderSource = std::stringstream();
		}
		else {
			std::string cleanLine = line;

			// TODO: Handle block comments

			// Strip line comments from current line
			std::smatch comment_match;
			if (std::regex_match(cleanLine, comment_match, line_comment)) {
				cleanLine = comment_match.prefix();
			}

			// Handle includes
			std::smatch include_match;
			if (std::regex_match(cleanLine, include_match, include_regex)) {
				std::string includePath = include_match[1].str();
				if (!std::filesystem::is_block_file(includePath)) {
					// Try to resolve relative
					includePath = resolveRelative(includePath, srcDir);
				}
				std::cout << "Found file to include: \"" << includePath << "\"" << std::endl;

				std::ifstream includeFile(includePath);
				if (!includeFile.is_open())
					throw std::runtime_error("Error: Could not open include file \"" + include_match[1].str() + "\"");

				sources.push_back(includePath);
				auto [includePrefix, includePipeline] = parseFile(includeFile, srcDir, sources);

				// If the current code is still in prefix we add passings
				if (currentShaderType == PREFIX) {
					prefix.passings.insert(prefix.passings.end(), includePrefix.passings.begin(), includePrefix.passings.end());
					prefix.code += "#line 1\n" + includePrefix.code;
				}
				else {
					// If the current code is not prefix, there should be no passings
					if (includePrefix.passings.size() != 0) {
						throw std::runtime_error("Error: Include adds passings in a shader context.");
					}

					// Add shaders defined in the include and check that they are not already defined
					for (auto  [stage, code] : includePipeline) {
						if (pipeline.find(stage) != pipeline.end()) {
							throw std::runtime_error("Error: Multiple definition for the same shader stage");
						}
						pipeline[stage] = code;
					}

					// If there are shader stages in the include we assume that the next line starts a new shader
					if (includePipeline.size() != 0) {
						currentShaderType = REQUIRE_SHADER;
					}

					// Add prefix code to current shader and add line directive
					shaderSource << "#line 0\n" << includePrefix.code << "\n";
				}
				shaderSource << "#line " << lineNumber + 1 << std::endl;

				continue;
			}
			else if (currentShaderType == REQUIRE_SHADER && cleanLine.size() != 0) {
				throw std::runtime_error("Error: Expected new shader type (since include defined shader types.)");
			}
			shaderSource << line << std::endl;
		}
		

	}
	if (currentShaderType == PREFIX) {
		prefix = parsePrefix(shaderSource.str());
		prefix.defines = defines;
	}
	else {
		pipeline[currentShaderType].code.back() = shaderSource.str();
	}
	return { prefix, pipeline };
}

#pragma endregion
gl::Shader::Shader() :
	mProgram(0),
	mLastUpdated(0)
{
}

Shader::Shader(std::string path) :
	Shader() 
{
	mSourceFiles.push_back(path);
}

gl::Shader::Shader(std::initializer_list<std::pair<GLenum, std::string>> stages) 
	: Shader()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << "Compiling shader" << std::endl;

	std::vector<GLuint> shaders;
	bool allValid = true;
	for (auto [type, src] : stages) {
		auto [valid, sid] = compileShader(src, type);
		shaders.push_back(sid);
		allValid = allValid && valid;
	}

	GLint success = 0;
	if (allValid) {
		// clean old program
		mProgram = glCreateProgram();
		for (GLuint shader : shaders) 
			glAttachShader(mProgram, shader);
		glLinkProgram(mProgram);
		for (GLuint shader : shaders) glDeleteShader(shader);

		glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetProgramInfoLog(mProgram, 1024, NULL, infoLog);
			std::cerr << "failed to link shader:\n" << infoLog << std::endl;
		}

		glValidateProgram(mProgram);
		glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			std::cerr << "failed to validate shader" << std::endl;
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	if (success && allValid) {
		std::cout << "Compilation sucessfull (Compile time: " << duration << "ms)" << std::endl;
	}

}

Shader::~Shader() {
	glDeleteProgram(mProgram);
}

ShaderRequirements gl::Shader::use()
{
	if (!mSourceFiles.empty() && requiresUpdate()) {
		update();
	}
	auto requirements = require();
	glUseProgram(mProgram);
	return requirements;
}

bool gl::Shader::requiresUpdate() const
{
	for (const std::string& srcFile : mSourceFiles) {
		if (std::filesystem::last_write_time(srcFile).time_since_epoch().count() > mLastUpdated)
			return true;
	}
	return false;
}

void Shader::update() {
	if (mSourceFiles.empty()) return;	// No name given -> shader was probably compield from constant char *

	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << "Compiling shader \"" << mSourceFiles[0] << "\"" << std::endl;

	for (const std::string& srcFile : mSourceFiles) {
		mLastUpdated = std::max(mLastUpdated, std::filesystem::last_write_time(srcFile).time_since_epoch().count());
	}


	// Read file
	std::ifstream in(mSourceFiles[0]);
	if (!in.is_open()) {
		std::cerr << "Error loading shaderfile " << mSourceFiles[0] << std::endl;
		mSourceFiles.clear();			// We will not try to update from this file again
		return;
	}

	mSourceFiles.resize(1);

	std::map<GLenum, ShaderCode> pipeline;
	Prefix prefix;

	// Get source folder of file
	std::string folder = std::filesystem::path(mSourceFiles[0]).parent_path().string();

	try {
		std::tie(prefix, pipeline) = parseFile(in, folder, mSourceFiles, mDefines);

		// Validate that all shaders required are found
		bool validPipeline = validatePipeline(pipeline);

		// Compile all shaders
		bool allShadersCompiled = true;
		GLenum previousShader = 0;
		auto getNextShaderStageInPipeline = [&](int stage) {
			int nextStage = stage + 1;
			for (; nextStage < 5; ++nextStage) {
				if (pipeline.find(ShaderPipeline[nextStage]) != pipeline.end())
					break;
			}
			return nextStage;
		};

		std::vector<GLuint> shaders;

		for (int shaderStage = 0; shaderStage < 5; shaderStage = getNextShaderStageInPipeline(shaderStage)) {
			GLenum currentShader = ShaderPipeline[shaderStage];
			auto [ret, shader] = loadAndCompileShader(
				pipeline[currentShader],
				currentShader,
				previousShader,
				prefix);
			if (ret) shaders.push_back(shader);
			allShadersCompiled &= ret;
			previousShader = currentShader;
		}

		// Link pipeline
		GLint success = 0;
		if (validPipeline && allShadersCompiled) {
			// clean old program
			glDeleteProgram(mProgram);
			mProgram = glCreateProgram();
			for (GLuint shader : shaders) glAttachShader(mProgram, shader);
			glLinkProgram(mProgram);
			for (GLuint shader : shaders) glDeleteShader(shader);

			glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetProgramInfoLog(mProgram, 1024, NULL, infoLog);
				std::cerr << "failed to link shader:\n" << infoLog << std::endl;
			}

			glValidateProgram(mProgram);
			glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &success);
			if (!success)
			{
				std::cerr << "failed to validate shader" << std::endl;
			}
		}

		auto t2 = std::chrono::high_resolution_clock::now();
		long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		if (success && allShadersCompiled && validPipeline) {
			std::cout << "Compilation sucessfull (Compile time: " << duration << "ms)" << std::endl;
		}
	}
	catch (std::runtime_error e) {
		std::cerr << e.what() << std::endl;
	}
	std::cout << "---------------------------------------------" << std::endl;
}

void gl::Shader::setDefine(const std::string& name, const std::string& value)
{
	mDefines[name] = value;
	// Force update
	mLastUpdated = 0;
}

void gl::Shader::setDefine(const std::string& name, const int value)
{
	setDefine(name, std::to_string(value));
}

void gl::Shader::setDefine(const std::string& name, float value)
{
	setDefine(name, std::to_string(value));
}

void gl::Shader::setDefineFlag(const std::string& name)
{
	setDefine(name, "");
}

void gl::Shader::removeDefine(const std::string& name)
{
	mDefines.erase(name);
}

bool gl::Shader::hasDefine(const std::string& name)
{
	return mDefines.find(name) != mDefines.end();
}
