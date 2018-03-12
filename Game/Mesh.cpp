#define _CRT_SECURE_NO_WARNINGS
#include  "Mesh.h"
#include<vector>
#include<fstream>
#include<iostream>
#include<glm\glm.hpp>

#define BUFFER_OFFSET(i) ((char*)0 + (i))

struct MeshFace
{
	MeshFace() {
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = 0;

		textureUVs[0] = 0;
		textureUVs[1] = 0;
		textureUVs[2] = 0;

		normals[0] = 0;
		normals[1] = 0;
		normals[2] = 0;
	}

	MeshFace(unsigned int v1, unsigned int v2, unsigned int v3,
		unsigned int t1, unsigned int t2, unsigned int t3,
		unsigned int n1, unsigned int n2, unsigned int n3)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] =v3;

		textureUVs[0] = t1;
		textureUVs[1] = t2;
		textureUVs[2] = t3;

		normals[0] = n1;
		normals[1] = n2;
		normals[2] = n3;
	}

	unsigned int vertices[3];
	unsigned int textureUVs[3];
	unsigned int normals[3];
};

Mesh::Mesh()
{

}
Mesh::~Mesh()
{

}
void Mesh::update() {

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * unpackedVertexData.size(), &unpackedVertexData[0]);

	// UVs
	glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * unpackedTextureData.size(), &unpackedTextureData[0]);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * unpackedNormalData.size(), &unpackedNormalData[0]);

	// Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

}

void Mesh::draw()
{
	vbo.draw();
}

void Mesh::createVBO()
{
	unsigned int numTris = (unsigned int)(vertices.size()) / 3; // todo: handle non-triangulated meshes

																// Setup VBO

																// Set up position (vertex) attribute
	if (vertices.size() > 0)
	{
		AttributeDescriptor positionAttrib;
		positionAttrib.attributeLocation = AttributeLocations::VERTEX;
		positionAttrib.attributeName = "vertex";
		positionAttrib.data = &vertices[0];
		positionAttrib.elementSize = sizeof(float);
		positionAttrib.elementType = GL_FLOAT;
		positionAttrib.numElements = numTris * 3 * 3; // (num triangles * three vertices per triangle * three floats per vertex)
		positionAttrib.numElementsPerAttrib = 3;
		vbo.addAttributeArray(positionAttrib);

	}

	// Set up UV attribute
	if (textureCoordinates.size() > 0)
	{
		AttributeDescriptor uvAttrib;
		uvAttrib.attributeLocation = AttributeLocations::TEX_COORD;
		uvAttrib.attributeName = "uv";
		uvAttrib.data = &textureCoordinates[0];
		uvAttrib.elementSize = sizeof(float);
		uvAttrib.elementType = GL_FLOAT;
		uvAttrib.numElements = numTris * 3 * 2;
		uvAttrib.numElementsPerAttrib = 2;
		vbo.addAttributeArray(uvAttrib);
	}

	// Set up normal attribute
	if (normals.size() > 0)
	{
		AttributeDescriptor normalAttrib;
		normalAttrib.attributeLocation = AttributeLocations::NORMAL;
		normalAttrib.attributeName = "normal";
		normalAttrib.data = &normals[0];
		normalAttrib.elementSize = sizeof(float);
		normalAttrib.elementType = GL_FLOAT;
		normalAttrib.numElements = numTris * 3 * 3;
		normalAttrib.numElementsPerAttrib = 3;
		vbo.addAttributeArray(normalAttrib);
	}

	// set up other attributes...

	vbo.createVBO(GL_STATIC_DRAW);
}
//Load a Mesh
bool Mesh::loadFromFile(const std::string &file)
{
	std::ifstream input;
	input.open(file);

	if (!input)
	{
		std::cout << "Mesh.loadFromFile : Could not open the file " << file<< std::endl;
		return false;
	}

	std::string inputString;

	//Unique Data
	std::vector<glm::vec3> vertexData;
	std::vector<glm::vec2> textureData;
	std::vector<glm::vec3> normalData;

	//Index / face data
	std::vector<MeshFace> faceData;
	//OpenGL ready data


	while (std::getline(input, inputString))
	{
		if (inputString[0] == '#') {
			//this line is a comment
			continue;
		}
		else if(inputString[0] =='v')
		{
			if (inputString[1] == ' ')
			{
				//vertex data
				glm::vec3 temp;

				std::sscanf(inputString.c_str(),"v %f %f %f", &temp.x, &temp.y, &temp.z);

				vertexData.push_back(temp);
			}
			else if (inputString[1] == 't')
			{
				//uv data
				glm::vec2 temp;

				std::sscanf(inputString.c_str(), "vt %f %f", &temp.x, &temp.y);

				textureData.push_back(temp);
			}
			else if (inputString[1] == 'n') 
			{
				//normal data
				glm::vec3 temp;

				std::sscanf(inputString.c_str(), "vn %f %f %f", &temp.x, &temp.y, &temp.z);

				normalData.push_back(temp);
			}
		}
		else if (inputString[0] == 'f')
		{
			//face data
			MeshFace temp;

			std::sscanf(inputString.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u", 
				&temp.vertices[0], &temp.textureUVs[0], &temp.normals[0],
				&temp.vertices[1], &temp.textureUVs[1], &temp.normals[1],
				&temp.vertices[2], &temp.textureUVs[2], &temp.normals[2]);

			faceData.push_back(temp);
		}
		else
		{
			//std::cout << "Line not recognized, skipping: " << inputString<<std::endl;
			continue;
		}
	}

	input.close();

	for (unsigned int i = 0; i < faceData.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].x);
			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].y);

			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);

		}
	}

	numFaces = faceData.size();
	numVertices = numFaces * 3;

	//sendd data to opengl
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboUVs);
	glGenBuffers(1, &vboNormals);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0); //vertex
	glEnableVertexAttribArray(1); //uvs
	glEnableVertexAttribArray(2); //normals

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unpackedVertexData.size(),&unpackedVertexData[0],GL_STATIC_DRAW);

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	//uvs
	glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unpackedTextureData.size(), &unpackedTextureData[0], GL_STATIC_DRAW);

	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unpackedNormalData.size(), &unpackedNormalData[0], GL_STATIC_DRAW);

	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	return true;
}

//release data
void Mesh::unload()
{
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboUVs);
	glDeleteBuffers(1, &vboVertices);

	glDeleteVertexArrays(1, &vao);

	vboNormals = NULL;
	vboUVs = NULL;
	vboVertices = NULL;
	vao = NULL;

	numFaces = 0;
	numVertices = 0;
}

unsigned int Mesh::getNumFaces() const
{
	return numFaces;
}
unsigned int Mesh::getNumVertices() const
{
	return numVertices;
}
