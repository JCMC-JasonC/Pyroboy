#include"LUT.h"
#include "GL/glut.h"
#define _CRT_SECURE_NO_WARNINGS

LUT::LUT()
{

}
LUT::LUT(std::string &file)
{
	loadLUT(file);
}
LUT::~LUT()
{

}
void LUT::print()
{
	for (int i = 0; i < lut.size(); i++)
	{
		std::cout << lut[i].r << " " << lut[i].g << " " << lut[i].b << std::endl;
	}
}

void LUT::bind()
{
	GLuint texture3D;

	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUTsize, LUTsize, LUTsize, 0, GL_RGB, GL_FLOAT, &lut[0]);
	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);
}
void LUT::loadLUT(std::string &file)
{
	std::ifstream LUTfile;
	LUTfile.open(file);
	while (!LUTfile.eof()) {
		std::string LUTline;
		getline(LUTfile, LUTline);
		if (LUTline.empty()) continue;
		RGB line;
		if (sscanf_s(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) lut.push_back(line);
	}
	LUTfile.close();
}