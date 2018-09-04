#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>  

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int ID;
	Shader(const GLchar* vertexPath, const GLchar* framentPath);

	void use();

	void setBool(const std::string &name, bool value)const;
	void setInt(const std::string &name, int value)const;
	void setFloat(const std::string &name, float value)const;
	void setMat4(const std::string &name, glm::mat4 value)const;
	void setVec3(const std::string &name, glm::vec3 value)const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x,value.y,value.z); 
	}

};

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	//1.���ļ�·���л����ɫ��
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ifstream�����׳��쳣��
	vShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	
	try
	{
		//���ļ�
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		//��ȡ�ļ��Ļ������ݵ���������
		vShaderStream <<vShaderFile.rdbuf();
		fShaderStream <<fShaderFile.rdbuf();

		//�ر��ļ�������
		vShaderFile.close();
		fShaderFile.close();

		//ת����������string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch(std::ifstream::failure e)
	{
		std::cout<<"error::shader::file_not_succesfully_read"<<std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//������ɫ��
	unsigned int vertex, fragment;
	int success;
	char inflog[512];

	//������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1,&vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, inflog);
		std::cout<<"error::shader::vertex::compilation_failed\n"<<std::endl;
	};

	//Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1,&fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, inflog);
		std::cout<<"error::shader::fragment::compilation_failed\n"<<std::endl;
	};

	//��ɫ������
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//��ӡ���Ӵ���
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, inflog);
		std::cout<<"error::program::fragment::compilation_failed\n"<<std::endl;
	};

	//ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{	
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
} 

void Shader::setMat4(const std::string &name, glm::mat4 value) const
{ 
	int projectionLoc = glGetUniformLocation(ID,  name.c_str());
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(value));
} 
#endif
