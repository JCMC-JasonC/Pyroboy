#pragma once
#include<GL/glew.h>
#include<string>

enum TextureType
{
	Diffuse,
	Specular
};
class Texture
{
public:
	Texture();
	~Texture();

	bool load(const std::string &file);
	void unload();

	void bind();
	void unbind();

	//handle to the texture object
	GLuint textObj = 0;

private:
};