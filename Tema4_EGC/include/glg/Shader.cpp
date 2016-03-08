#include "Shader.hpp"

glg::Shader::Shader(const GLchar * inGlsl)
{
	std::ifstream glslFile(inGlsl);
	if (!glslFile.good())
		throw std::runtime_error("Cannot open the GLSL file! The file does not exists or the application does not have access to it!");

	glslFile.seekg(0, glslFile.end);
	source.resize(glslFile.tellg());
	glslFile.seekg(0, glslFile.beg);
	glslFile.read(&source[0], source.size());
	glslFile.close();
}

void glg::Shader::compile()
{
	if (compiled)
		throw std::runtime_error("Shader object is already compiled!");
	if (source.empty())
		throw std::runtime_error("Shader source is empty!");

	createShader();
	GLint success;
	GLchar infoLog[INFO_BUFF_SIZE];

	const GLchar* src_ptr = source.c_str();
	const GLint src_siz = source.size();
	glShaderSource(shader, 1, &src_ptr, &src_siz);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) 
	{
		glGetShaderInfoLog(shader, INFO_BUFF_SIZE, nullptr, infoLog);
		std::string error("Failed to compile the shader!\n");
		error.append(infoLog);
		throw std::runtime_error(error);
	}
	compiled = GL_TRUE;
}

void glg::VertexShader::createShader()
{
	shader = glCreateShader(GL_VERTEX_SHADER);
	if (shader == 0)
		throw std::runtime_error("Failed to create the vertex shader!");
}

void glg::FragmentShader::createShader()
{
	shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader == 0)
		throw std::runtime_error("Failed to create the fragment shader!");
}

glg::ShaderProgram::ShaderProgram(std::initializer_list<Shader*> shader)
{
	createProgram();
	for (auto& shdr : shader)
		shaders.push_back(shdr->shader);
}

void glg::ShaderProgram::attachShader(Shader & shdr)
{
	if (linked)
		throw std::runtime_error("Cannot attach new shaders! This program has been linked!");
	shaders.push_back(shdr.shader);
}

void glg::ShaderProgram::link()
{
	if (shaders.empty())
		throw std::runtime_error("This program has no shaders attached to it or it has been linked!");

	GLint success;
	GLchar infoLog[INFO_BUFF_SIZE];

	for (auto& shader : shaders)
		glAttachShader(program, shader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, INFO_BUFF_SIZE, nullptr, infoLog);
		std::string error("Failed to link the program!\n");
		error.append(infoLog);
		throw std::runtime_error(error);
	}
	
	for (auto& shdr : shaders)
		glDetachShader(program, shdr);
	
	linked = GL_TRUE;
	shaders.clear();
}

void glg::ShaderProgram::use()
{
	if (!linked)
		throw std::runtime_error("You must link the program first!");
	glUseProgram(program);
}

void glg::ShaderProgram::uniform(const GLchar * name, const Light * light)
{
	std::string base = name;
	uniform((base + std::string(".position")).c_str(), light->position);
	uniform((base + std::string(".direction")).c_str(), light->direction);
	uniform((base + std::string(".color")).c_str(), light->color);
	uniform((base + std::string(".cutOff")).c_str(), light->cutOff);
	uniform((base + std::string(".outerCutOff")).c_str(), light->outerCutOff);
	uniform((base + std::string(".ambient")).c_str(), light->ambient);
	uniform((base + std::string(".specular")).c_str(), light->specular);
	uniform((base + std::string(".shininess")).c_str(), light->shininess);
	uniform((base + std::string(".constant")).c_str(), light->constant);
	uniform((base + std::string(".linear")).c_str(), light->linear);
	uniform((base + std::string(".quadratic")).c_str(), light->quadratic);
	uniform((base + std::string(".type")).c_str(), light->type);
	uniform((base + std::string(".blinn")).c_str(), light->blinn);
	uniform((base + std::string(".atten")).c_str(), light->atten);
	uniform((base + std::string(".off")).c_str(), light->off);
}

void glg::ShaderProgram::createProgram()
{
	program = glCreateProgram();
	if (program == 0)
		throw std::runtime_error("Failed to create program!");
}

GLint glg::ShaderProgram::getUniformLocation(const GLchar * name)
{
	GLint location = glGetUniformLocation(program, name);
	if (location == -1)
		throw std::runtime_error("Uniform location not found!");
	return location;
}
