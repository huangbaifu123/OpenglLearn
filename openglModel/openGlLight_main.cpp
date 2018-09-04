#include <glad/glad.h>  //GLAD的头文件包含了正确的OpenGL头文件 需要先包含
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

//纹理加载
unsigned int loadTexture(char* const path);
unsigned int loadCubeTexture(vector<std::string> faces);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//鼠标坐标
float lastX, lastY;
bool firstMouse = true;

//渲染时间
float lastTime = 0.0f;
float deltaTime = 0.0f;

//摄像机类
Camera camera(glm::vec3(-0.2f, 0.5f, 4.5f), glm::vec3(0.0f, 1.0f,  0.0f));

//光源位置
glm::vec3 lightPos(0.2f, 0.5f, 3.0f);

int main()  
{  
    glfwInit();  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
  


	//GLFW创建窗口和控制
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "testwindow", nullptr, nullptr);
	if (window == NULL){
		std::cout<<"falid to create GLFW window"<<std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//鼠标移动回调
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//GLAD管理opengl函数,调用任何OpenGL函数之前需要初始化GLAD
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

	//创建帧缓冲
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//创建纹理绑定到帧缓冲
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//附加到绑定的帧缓冲对象
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	//检测是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout<<"ERROR::FRAMEBUFFER:: Framebuffer is not complete!"<<endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//创建渲染缓冲对象绑定到帧缓冲
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

	//天空盒子
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

	//加载纹理
	unsigned int cubeTextureId = loadTexture("res/marble.jpg");
	unsigned int plantTextureId = loadTexture("res/metal.png");
	unsigned int grassTextureId = loadTexture("res/grass.png");
	unsigned int windowTextureId = loadTexture("res/window.png");
	//加载天空盒子纹理
	vector<std::string> faces;
	faces.push_back("res/skybox/right.jpg");
	faces.push_back("res/skybox/left.jpg");
	faces.push_back("res/skybox/bottom.jpg");
	faces.push_back("res/skybox/top.jpg");
	faces.push_back("res/skybox/front.jpg");
	faces.push_back("res/skybox/back.jpg");

	unsigned int skyTextureId = loadCubeTexture(faces);
	//渲染循环
	while(!glfwWindowShouldClose(window)) 
	{
		//光源位置
		float timeValue = glfwGetTime();
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = glfwGetTime();
		


		//按键输入
		putKeyCallback(window);

		//渲染指令
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		//将帧缓冲设置为自定义，并设置环境
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲
		glEnable(GL_DEPTH_TEST);

//--------天空盒子-----------//
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
//-------天空盒子end------//


//---------模型加载------------------//
		//ourShader.use();
		//ourShader.setVec3("viewPos",  camera.cameraPos);//观察者位置

		////点光效果s
		//ourShader.setVec3("spotLight.direction",  camera.cameraFront);
		//ourShader.setFloat("spotLight.cutoff",  glm::cos(glm::radians(12.5f)));
		//ourShader.setFloat("spotLight.outerCutOff",  glm::cos(glm::radians(17.0f)));
  //      ourShader.setVec3("spotLight.ambient",  glm::vec3 (0.2f, 0.2f, 0.2f));
  //      ourShader.setVec3("spotLight.diffuse",  glm::vec3 (0.5f, 0.5f, 0.5f));
		//ourShader.setVec3("spotLight.lightPos",  camera.cameraPos);//摄像机作光源位置
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
//---------模型加载结束--------------//

//----------深度测试 模板测试-------------//
		//depthShader.use();

		////先进行模板缓冲测试，在进行深度测试
		//glEnable(GL_DEPTH_TEST);	//深度测试
		//glEnable(GL_STENCIL_TEST);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//
		////地板
		//glStencilMask(0x00);
		//myPlant.Draw(depthShader, camera, plantTextureId, glm::vec3(0.0f, 0.0f, 0.0f));
		////myPlant.Draw(depthShader, camera, plantTextureId, glm::vec3(0.0f, 0.5f, 0.0f));
		//
		////方块	
		//
		//glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CW);
		//myCube.Draw(depthShader, outLineShader, camera, cubeTextureId, glm::vec3(0.0f, 0.01f, 0.0f));
		//myCube.Draw(depthShader, outLineShader,camera, cubeTextureId, glm::vec3(1.0f, 0.01f, -3.0f));	
		//glDisable(GL_CULL_FACE);

		////结束模板测试
		//glDisable(GL_STENCIL_TEST);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//
		////距离容器
		//std::map<float, glm::vec3> sorted;
		////根据摄像机位置，更新透明物体绘制顺序
		//for (int i = 0; i < vegetation.size() ; i++)
		//{
		//	float distance = glm::length(camera.cameraPos - vegetation[i]);
		//	sorted[distance] = vegetation[i];
		//}

		//for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		//{
		//	myCube.DrawOneFace(mixShader,camera, windowTextureId, it->second);	
		//}

//----------深度测试结束-------------//

//-------------帧缓冲-------------//
		//绘制场景结束后，返回默认，即将附加纹理绘制到主屏幕上
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT);

		FrameShader.use();
		glBindVertexArray(frameVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 6);
//-------------帧缓冲-------------//

		glfwSwapBuffers(window);//双缓冲
		glfwPollEvents();//检查事件触发	
		
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