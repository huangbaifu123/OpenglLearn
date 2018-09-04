#ifndef CUBE_H
#define CUBE_H
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "openGlShaders.h"
#include "Mesh.h"
#include"camera.h"

#include <string>
#include<vector>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
class Shader;
class Cube
{
public:
	unsigned int cubeVAO, cubeVBO;
	bool is_border;
	bool is_all_connect_border;
	Shader *outLineShader;
	~Cube(){
		delete outLineShader;
	};
	Cube(){
			is_border = false;	  //开启边框绘制
			is_all_connect_border = false;	//是否连接所有边框
			float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left    
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		// Right face
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left        
		- 0.5f,  0.5f, -0.5f,  0.0f, 1.0f // top-left
		}; 
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	void DrawOneFace(Shader shader, Camera camera, unsigned int cubeTextureId, glm::vec3 translateVec ){
		shader.use();
	    glm::mat4 model;
		glm::mat4 view = camera.getCameraVeiw();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        model = glm::translate(model, translateVec);
        shader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTextureId);
		shader.setInt("texture1", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void Draw(Shader shader, Shader outShader, Camera camera, unsigned int cubeTextureId, glm::vec3 translateVec ){

        glm::mat4 model;
		glm::mat4 view = camera.getCameraVeiw();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        model = glm::translate(model, translateVec);
        shader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTextureId);
		shader.setInt("texture1", 0);


		if (is_border)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);;//物体片段被渲染时（设为总是通过），将模板缓冲更新为1
			glStencilMask(0xff);
			shader.use();
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));//重复绘制时，重置model大小
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			
			outShader.use();
			model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
			outShader.setMat4("view", view);
			outShader.setMat4("projection", projection);
			outShader.setMat4("model", model);

			glStencilFunc(GL_NOTEQUAL, 1, 0xff); //物体片段被渲染时，不等于1的部分（上一个立方体绘制时将片段模板缓冲更新为1）被绘制，其他部分丢弃
			
			//为了模板缓冲不被改变
			glStencilMask(0x00);//禁止写入模板缓冲
			if (is_all_connect_border)//需要所有轮廓链接时，关闭深度测试，丢弃挡在其他物体前的轮廓-与绘制的先后顺序有关
				{
					glDisable(GL_DEPTH_TEST);//不开启深度测试，即深度测试结果总为不通过
				}
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//-------end

			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			
			if (!is_all_connect_border)
				glClear(GL_STENCIL_BUFFER_BIT);//清除模板缓冲
		}
		else
			glDrawArrays(GL_TRIANGLES, 0, 36);

		shader.use();
	}

private:
};
#endif