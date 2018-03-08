#include "Texture.h"
#include<SOIL/SOIL.h>
#include<iostream>

Texture::Texture()
{

}
Texture::~Texture()
{
	unload();
}

void Texture::load(const std::string &file)
{
											//let soil decide/  dont want to reuse
	textObj = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);//mipmaps are multiple images of the same object, all scaled differently so that when we are farther away it would use a smaller image

	if (textObj == NULL)
	{
		std::cout << "Texture failed to load." << std::endl << SOIL_last_result() << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, textObj);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//u axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//v axis
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
void Texture::unload()
{
	if (textObj != 0)
	{
		glDeleteTextures(1, &textObj);
		textObj = 0;
	}
}
void Texture::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textObj);
}
void Texture::unbind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, textObj);
}
void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}