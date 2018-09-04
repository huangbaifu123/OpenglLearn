#include <glad/glad.h>  //GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ� ��Ҫ�Ȱ���
#include "openGlShaders.h"
#include"model.h"
#include "camera.h"
#include"cube.h"
#include"plant.h"

#include "stb_image.h"

#include<map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
float mix_param = 0.0;

void putKeyCallback(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//�������
unsigned int loadTexture(char* const path);
unsigned int loadCubeTexture(vector<std::string> faces);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//�������
float lastX, lastY;
bool firstMouse = true;

//��Ⱦʱ��
float lastTime = 0.0f;
float deltaTime = 0.0f;

//�������
Camera camera(glm::vec3(-0.2f, 0.5f, 4.5f), glm::vec3(0.0f, 1.0f,  0.0f));

//��Դλ��
glm::vec3 lightPos(0.2f, 0.5f, 3.0f);

int main()  
{  
    glfwInit();  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
  


	//GLFW�������ںͿ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "testwindow", nullptr, nullptr);
	if (window == NULL){
		std::cout<<"falid to create GLFW window"<<std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//����ƶ��ص�
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//GLAD����opengl����,�����κ�OpenGL����֮ǰ��Ҫ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout<<"falid to init flad"<<std::endl;
		return -1;
	}
	glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);

	
	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(2.0f, 0.0f, -2.9f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, -1.7f));
	vegetation.push_back(glm::vec3(1.7f, 0.0f, -1.0f));
	vegetation.push_back(glm::vec3(1.0f, 0.0f, -0.5f));


	Shader ourShader("shader.vs", "shader.mfs");
	Shader depthShader("depth_tesing.vs", "depth_tesing.mfs");
	Shader outLineShader("depth_tesing.vs", "outlineShader.mfs");

	Shader mixShader("depth_tesing.vs", "depth_tesing.mfs");

	Shader FrameShader("4.5.frameShader.vs", "4.5.frameShader.mfs");

	Shader skyShader("sky.shader.vs", "sky.shader.mfs");
	//Model ourModel("res/nanosuit/nanosuit.obj");
	Cube myCube;
	Plant myPlant;

	//����֡����
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//��������󶨵�֡����
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//���ӵ��󶨵�֡�������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	//����Ƿ�����
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout<<"ERROR::FRAMEBUFFER:: Framebuffer is not complete!"<<endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//������Ⱦ�������󶨵�֡����
	unsigned int renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float frameVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int frameVAO, frameVBO;
	glGenVertexArrays(1, &frameVAO);
	glGenBuffers(1, &frameVBO);
	glBindVertexArray(frameVAO);
	glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4* sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	//��պ���
	    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	unsigned int skyVAO, skyVBO;
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
	glBindVertexArray(0);

	//��������
	unsigned int cubeTextureId = loadTexture("res/marble.jpg");
	unsigned int plantTextureId = loadTexture("res/metal.png");
	unsigned int grassTextureId = loadTexture("res/grass.png");
	unsigned int windowTextureId = loadTexture("res/window.png");
	//������պ�������
	vector<std::string> faces;
	faces.push_back("res/skybox/right.jpg");
	faces.push_back("res/skybox/left.jpg");
	faces.push_back("res/skybox/bottom.jpg");
	faces.push_back("res/skybox/top.jpg");
	faces.push_back("res/skybox/front.jpg");
	faces.push_back("res/skybox/back.jpg");

	unsigned int skyTextureId = loadCubeTexture(faces);
	//��Ⱦѭ��
	while(!glfwWindowShouldClose(window)) 
	{
		//��Դλ��
		float timeValue = glfwGetTime();
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = glfwGetTime();
		


		//��������
		putKeyCallback(window);

		//��Ⱦָ��
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		//��֡��������Ϊ�Զ��壬�����û���
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �������ڲ�ʹ��ģ�建��
		glEnable(GL_DEPTH_TEST);

//--------��պ���-----------//
		skyShader.use();
		skyShader.setInt("skybox", 0);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.getCameraVeiw();
		skyShader.setMat4("projection", projection);
		skyShader.setMat4("view", view);

		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);

        glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
//-------��պ���end------//


//---------ģ�ͼ���------------------//
		//ourShader.use();
		//ourShader.setVec3("viewPos",  camera.cameraPos);//�۲���λ��

		////���Ч��s
		//ourShader.setVec3("spotLight.direction",  camera.cameraFront);
		//ourShader.setFloat("spotLight.cutoff",  glm::cos(glm::radians(12.5f)));
		//ourShader.setFloat("spotLight.outerCutOff",  glm::cos(glm::radians(17.0f)));
  //      ourShader.setVec3("spotLight.ambient",  glm::vec3 (0.2f, 0.2f, 0.2f));
  //      ourShader.setVec3("spotLight.diffuse",  glm::vec3 (0.5f, 0.5f, 0.5f));
		//ourShader.setVec3("spotLight.lightPos",  camera.cameraPos);//���������Դλ��
  //      ourShader.setVec3("spotLight.specular", glm::vec3 (1.0f, 1.0f, 1.0f));

	 //   // view/projection transformations
		//ourShader.setMat4("projection", projection);
		//ourShader.setMat4("view", view);

		//// render the loaded model
		//glm::mat4 model;
		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		//ourShader.setMat4("model", model);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//ourModel.Draw(ourShader);
//---------ģ�ͼ��ؽ���--------------//

//----------��Ȳ��� ģ�����-------------//
		//depthShader.use();

		////�Ƚ���ģ�建����ԣ��ڽ�����Ȳ���
		//glEnable(GL_DEPTH_TEST);	//��Ȳ���
		//glEnable(GL_STENCIL_TEST);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//
		////�ذ�
		//glStencilMask(0x00);
		//myPlant.Draw(depthShader, camera, plantTextureId, glm::vec3(0.0f, 0.0f, 0.0f));
		////myPlant.Draw(depthShader, camera, plantTextureId, glm::vec3(0.0f, 0.5f, 0.0f));
		//
		////����	
		//
		//glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CW);
		//myCube.Draw(depthShader, outLineShader, camera, cubeTextureId, glm::vec3(0.0f, 0.01f, 0.0f));
		//myCube.Draw(depthShader, outLineShader,camera, cubeTextureId, glm::vec3(1.0f, 0.01f, -3.0f));	
		//glDisable(GL_CULL_FACE);

		////����ģ�����
		//glDisable(GL_STENCIL_TEST);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//
		////��������
		//std::map<float, glm::vec3> sorted;
		////���������λ�ã�����͸���������˳��
		//for (int i = 0; i < vegetation.size() ; i++)
		//{
		//	float distance = glm::length(camera.cameraPos - vegetation[i]);
		//	sorted[distance] = vegetation[i];
		//}

		//for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		//{
		//	myCube.DrawOneFace(mixShader,camera, windowTextureId, it->second);	
		//}

//----------��Ȳ��Խ���-------------//

//-------------֡����-------------//
		//���Ƴ��������󣬷���Ĭ�ϣ���������������Ƶ�����Ļ��
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT);

		FrameShader.use();
		glBindVertexArray(frameVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 6);
//-------------֡����-------------//

		glfwSwapBuffers(window);//˫����
		glfwPollEvents();//����¼�����	
		
	}
	glfwTerminate();
    return 0; 
} 

void putKeyCallback(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	camera.processInput(window, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char* const path){
	unsigned int textureID;
	glGenTextures(1, &textureID);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)		
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


unsigned int loadCubeTexture(vector<std::string> faces){
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		cout<<faces[i].c_str()<<endl;
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)		
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
	
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << faces[i].c_str() << std::endl;
			stbi_image_free(data);
		}
	}
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}