#pragma once
#include<GL/glew.h>
#include<glm\glm.hpp>
#include<string>

class ShaderProgram 
{
public:
	ShaderProgram();
	~ShaderProgram();

	//loads a vertex and fragment shader
	bool load(const std::string &vertFile, const std::string &fragFile);
	void load(const std::string &vertFile, const std::string &fragFile, const std::string &geoFile);

	bool isLoaded() const;

	//clear all data from opengl
	void unload();

	bool linkProgram();

	//use shader
	void bind() const;
	//detatch shader
	void unbind();

	//requires a re-link b4 it will be registered
	void addAttribute(unsigned int index, const std::string &attribName);

	//attribute is data that applies per vertex
	int getAttributeLocation(const std::string &name);

	//uniform is something that changes the object as a whole
	int getUniformLocation(const std::string &uniformName);

	//send data to shaders
	void sendUniform(const std::string &name, int integer);
	void sendUniform(const std::string &name, unsigned int unsignedInteger);
	void sendUniform(const std::string &name, float scalar);
	void sendUniform(const std::string &name, const glm::vec2 &vector);
	void sendUniform(const std::string &name, const glm::vec3 &vector);
	void sendUniform(const std::string &name, const glm::vec4 &vector);
	void sendUniformMat3(const std::string &name, float* matrix, bool transpose);
	void sendUniformMat4(const std::string &name, float* matrix, bool transpose);

private:
	bool loaded = false;

	GLuint vertexShader = 0;
	GLuint fragShader = 0;
	GLuint geoShader = 0;
	GLuint program = 0;

	std::string readFile(const std::string &fileName) const;

	bool compileShader(GLuint shader)const;

	void outputShaderLog(GLuint shader) const;
	void outputProgramLog()const;
};