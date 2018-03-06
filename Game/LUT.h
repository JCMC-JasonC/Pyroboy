#pragma once
#include<fstream>
#include<vector>
#include<string>
#include <iostream>
#include <GL\glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include <glm\gtx\color_space.hpp>
#include <iostream>

struct RGB
{
	float r;
	float g;
	float b;
};

class LUT
{
public:
	LUT();
	LUT(std::string &file);
	~LUT();

	void loadLUT(std::string &file);
	void print();
	void bind();
	std::vector<RGB> lut;
private:
	float LUTsize = 64.f;
};