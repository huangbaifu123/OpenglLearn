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
	//1.从文件路径中获得着色器
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ifstream对象抛出异常；
	vShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	
	try
	{
		//打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		//读取文件的缓冲内容到数据流中
		vShaderStream <<vShaderFile.rdbuf();
		fShaderStream <<fShaderFile.rdbuf();

		//关闭文件管理器
		vShaderFile.close();
		fShaderFile.close();

		//转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch(std::ifstream::failure e)
	{
		std::cout<<"error::shader::file_not_succesfully_read"<<std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//编译着色器
	unsigned int vertex, fragment;
	int success;
	char inflog[512];

	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1,&vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, inflog);
		std::cout<<"error::shader::vertex::compilation_failed\n"<<std::endl;
	};

	//片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1,&fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, inflog);
		std::cout<<"error::shader::fragment::compilation_failed\n"<<std::endl;
	};

	//着色器程序
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	//打印链接错误
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, inflog);
		std::cout<<"error::program::fragment::compilation_failed\n"<<std::endl;
	};

	//删除着色器
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
