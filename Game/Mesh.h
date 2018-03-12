#pragma once
#include<string>
#include<GL/glew.h>
#include<glm\glm.hpp>
#include"VertexBufferObject.h"
#include<vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	//Load a Mesh
	bool loadFromFile(const std::string &file);
	void draw();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<glm::vec4> colours;

	VertexBufferObject vbo;

	//release data
	void unload();
	void update();
	void createVBO();
	std::vector<float> unpackedVertexData;
	std::vector<float> unpackedTextureData;
	std::vector<float> unpackedNormalData;

	unsigned int getNumFaces() const;
	unsigned int getNumVertices() const;

	//OpenGL buffers and objects

	//vertex buffer objecrs
	GLuint vboVertices = 0;
	GLuint vboUVs = 0;
	GLuint vboNormals = 0;
	GLuint vao = 0;

private:
	unsigned int numFaces = 0;
	unsigned int numVertices = 0;

};