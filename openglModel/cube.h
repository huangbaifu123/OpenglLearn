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
			is_border = false;	  //�����߿����
			is_all_connect_border = false;	//�Ƿ��������б߿�
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
			glStencilFunc(GL_ALWAYS, 1, 0xFF);;//����Ƭ�α���Ⱦʱ����Ϊ����ͨ��������ģ�建�����Ϊ1
			glStencilMask(0xff);
			shader.use();
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));//�ظ�����ʱ������model��С
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			
			outShader.use();
			model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
			outShader.setMat4("view", view);
			outShader.setMat4("projection", projection);
			outShader.setMat4("model", model);

			glStencilFunc(GL_NOTEQUAL, 1, 0xff); //����Ƭ�α���Ⱦʱ��������1�Ĳ��֣���һ�����������ʱ��Ƭ��ģ�建�����Ϊ1�������ƣ��������ֶ���
			
			//Ϊ��ģ�建�岻���ı�
			glStencilMask(0x00);//��ֹд��ģ�建��
			if (is_all_connect_border)//��Ҫ������������ʱ���ر���Ȳ��ԣ�����������������ǰ������-����Ƶ��Ⱥ�˳���й�
				{
					glDisable(GL_DEPTH_TEST);//��������Ȳ��ԣ�����Ȳ��Խ����Ϊ��ͨ��
				}
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//-------end

			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			
			if (!is_all_connect_border)
				glClear(GL_STENCIL_BUFFER_BIT);//���ģ�建��
		}
		else
			glDrawArrays(GL_TRIANGLES, 0, 36);

		shader.use();
	}

private:
};
#endif