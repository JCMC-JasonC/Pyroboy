#pragma once
#include<GL/glew.h>
#include<string>

enum TextureType
{
	Diffuse,
	Specular,
	Normal
};
class Texture
{
public:
	Texture();
	~Texture();

	void load(const std::string &file);
	void unload();

	void bind();
	void unbind();

	void bind(GLenum textureUnit);
	void unbind(GLenum textureUnit);

	//handle to the texture object
	GLuint textObj = 0;

private:
};