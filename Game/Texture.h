#pragma once
#include<GL/glew.h>
#include<string>
#include<memory>

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
	typedef std::shared_ptr<Texture> Ptr;

	void load(const std::string &file);
	void createTexture(int width, int height, GLenum target, GLenum filtering, GLenum edgeBehaviour
		, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void* newDataPtr);
	void unload();

	void bind();
	void unbind();

	void bind(GLenum textureUnit);
	void unbind(GLenum textureUnit);

	//handle to the texture object
	GLuint textObj = 0;
private:
};