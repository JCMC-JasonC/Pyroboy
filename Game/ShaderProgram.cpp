#include "ShaderProgram.h"
#include<fstream>
#include<iostream>
#include<vector>

ShaderProgram::ShaderProgram()
{

}
ShaderProgram::~ShaderProgram()
{
	if (loaded)
	{
		unload();
	}
}

bool ShaderProgram::load(const std::string &vertFile, const std::string &fragFile, const std::string &geoFile)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	geoShader = glCreateShader(GL_GEOMETRY_SHADER);

	program = glCreateProgram();

	std::string source = readFile(vertFile);
	const GLchar *temp = static_cast<const GLchar*>(source.c_str());
	glShaderSource(vertexShader, 1, &temp, NULL);

	source = readFile(fragFile);
	temp = static_cast<const GLchar*>(source.c_str());
	glShaderSource(fragShader, 1, &temp, NULL);

	source = readFile(geoFile);
	temp = static_cast<const GLchar*>(source.c_str());
	glShaderSource(geoShader, 1, &temp, NULL);

	if (!compileShader(vertexShader))
	{
		std::cout << "Vertex shader failed to compile" << std::endl;

		outputShaderLog(vertexShader);
		unload();

		return false;
	}
	if (!compileShader(fragShader))
	{
		std::cout << "Fragment shader failed to compile" << std::endl;

		outputShaderLog(fragShader);
		unload();

		return false;
	}
	if (!compileShader(geoShader))
	{
		std::cout << "Geometry shader failed to compile" << std::endl;

		outputShaderLog(fragShader);
		unload();

		return false;
	}

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glAttachShader(program, geoShader);

	if (!linkProgram())
	{
		std::cout << "Shader program failed to link" << std::endl;

		outputProgramLog();
		unload();

		return false;
	}

	loaded = true;
	return true;
}
//loads a vertex and fragment shader
bool ShaderProgram::load(const std::string &vertFile, const std::string &fragFile)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	program = glCreateProgram();

	std::string source = readFile(vertFile);
	const GLchar *temp= static_cast<const GLchar*>(source.c_str());
	glShaderSource(vertexShader, 1, &temp, NULL);

	source = readFile(fragFile);
	temp = static_cast<const GLchar*>(source.c_str());
	glShaderSource(fragShader, 1, &temp, NULL);

	if (!compileShader(vertexShader))
	{
		std::cout << "Vertex shader failed to compile"<<std::endl;

		outputShaderLog(vertexShader);
		unload();

		return false;
	}
	if (!compileShader(fragShader))
	{
		std::cout << "Vertex shader failed to compile" << std::endl;

		outputShaderLog(fragShader);
		unload();

		return false;
	}

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);

	if (!linkProgram())
	{
		std::cout << "Shader program failed to link"<< std::endl;

		outputProgramLog();
		unload();

		return false;
	}

	loaded = true;
	return true;
}

bool ShaderProgram::isLoaded() const 
{
	return loaded;
}

//clear all data from opengl
void ShaderProgram::unload()
{
	if (vertexShader != 0) 
	{
		glDetachShader(program, vertexShader);
		glCreateShader(vertexShader);
		vertexShader = 0;
	}
	
	if (fragShader != 0)
	{
		glDetachShader(program, fragShader);
		glCreateShader(fragShader);
		fragShader = 0;
	}
	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}

	loaded = false;
}

bool ShaderProgram::linkProgram()
{
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	return success == GL_TRUE;
}

//use shader
void ShaderProgram::bind() const
{
	glUseProgram(program);
}
//detatch shader
void ShaderProgram::unbind()
{
	glUseProgram(GL_NONE);
}

//requires a re-link b4 it will be registered
void ShaderProgram::addAttribute(unsigned int index, const std::string &attribName)
{
	glBindAttribLocation(program, index, attribName.c_str());
}

//attribute is data that applies per vertex
int ShaderProgram::getAttributeLocation(const std::string &name)
{
	return glGetAttribLocation(program, name.c_str());
}

//uniform is something that changes the object as a whole
int ShaderProgram::getUniformLocation(const std::string &uniformName)
{
	//slow unless cached
	return glGetUniformLocation(program, uniformName.c_str());
}

//send data to shaders
void ShaderProgram::sendUniform(const std::string &name, int integer)
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, integer);
}
void ShaderProgram::sendUniform(const std::string &name, unsigned int unsignedInteger)
{
	GLint location = getUniformLocation(name);
	glUniform1ui(location, unsignedInteger);
}
void ShaderProgram::sendUniform(const std::string &name, float scalar)
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, scalar);
}
void ShaderProgram::sendUniform(const std::string &name, const glm::vec2 &vector)
{
	GLint location = getUniformLocation(name);
	glUniform2f(location, vector.x, vector.y);
}
void ShaderProgram::sendUniform(const std::string &name, const glm::vec3 &vector)
{
	GLint location = getUniformLocation(name);
	glUniform3f(location, vector.x, vector.y,vector.z);
}
void ShaderProgram::sendUniform(const std::string &name, const glm::vec4 &vector)
{
	GLint location = getUniformLocation(name);
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}
void ShaderProgram::sendUniformMat3(const std::string &name, float* matrix, bool transpose)
{
	GLint location = getUniformLocation(name);
	glUniformMatrix3fv(location, 1, transpose, matrix);
}
void ShaderProgram::sendUniformMat4(const std::string &name, float* matrix, bool transpose)
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, transpose, matrix);
}

std::string ShaderProgram::readFile(const std::string &fileName) const 
{
	std::ifstream input(fileName);

	if (!input)
	{
		std::cout << "Shader file not found: "<< fileName<<std::endl;
		return "";
	}

	std::string data(std::istreambuf_iterator<char>(input), (std::istreambuf_iterator<char>()));

	return data;
}

bool ShaderProgram::compileShader(GLuint shader)const
{
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	return success == GL_TRUE;
}

void ShaderProgram::outputShaderLog(GLuint shader) const
{
	std::string infoLog;

	GLint infoLen;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	infoLog.resize(infoLen);

	glGetShaderInfoLog(shader, infoLen, &infoLen, &infoLog[0]);

	std::cout << infoLog << std::endl;
}
void ShaderProgram::outputProgramLog()const
{
	std::string infoLog;

	GLint infoLen;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

	infoLog.resize(infoLen);

	glGetProgramInfoLog(program, infoLen, &infoLen, &infoLog[0]);

	std::cout << infoLog << std::endl;
}