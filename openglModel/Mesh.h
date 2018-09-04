#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "openGlShaders.h"

#include <string>
#include<vector>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	aiString path;  // ���Ǵ��������·������������������бȽ�
	string type;
};

class Mesh
{
public:
	//����
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	//����
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

Mesh::Mesh(vector<Vertex> vertices,vector<unsigned int> indices, 	vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(  GL_ELEMENT_ARRAY_BUFFER , EBO);

	glBufferData(GL_ARRAY_BUFFER,  vertices.size() * sizeof(Vertex), &vertices[0],  GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices.size() * sizeof(unsigned int), &indices[0],  GL_STATIC_DRAW);
	
	//����
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (void*)0);

	//���㷨��
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (void*)offsetof(Vertex, Normal));

	//��������
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		//�������
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = to_string(diffuseNr ++);
		else if (name == "specular_diffuse")
			number = to_string(specularNr ++);

		shader.setFloat("material." + name + number, i);
		glBindTexture(GL_TEXTURE_2D,  textures[i].id);//Ϊ���������Ԫ��λ�ã�������
	}
	
	 // ��������
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,  indices.size(),  GL_UNSIGNED_INT,  0);
    glBindVertexArray(0);

	//��������ΪĬ��0
	glActiveTexture(GL_TEXTURE0);
}



#endif 