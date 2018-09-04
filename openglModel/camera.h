#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>  


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float LOOKDISTAND = -1.0f;

class Camera
{
public:

	Camera(glm::vec3 cameraPos, glm::vec3 sceneUpVec);

public:
	glm::mat4 getCameraVeiw();
	void processInput(GLFWwindow* window, float deltaTime);
	void ProcessMouseMovement(double xoffset, double yoffset);
	
	void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    };
public:
    float Yaw;
    float Pitch;	
    float Zoom;	
	glm::mat4 veiw;
	glm::vec3 cameraUp;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;

	void setView(glm::vec3 cameraPos);

};

Camera::Camera(glm::vec3 _cameraPos,glm::vec3 _sceneUpVec){
	cameraPos = _cameraPos;
	cameraUp = _sceneUpVec;
	Zoom = ZOOM;
	Yaw = YAW;
	Pitch = PITCH;

	cameraFront = glm::vec3(0.0f, 0.0f, LOOKDISTAND);
}

glm::mat4 Camera::getCameraVeiw(){
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::processInput(GLFWwindow* window, float deltaTime){
	float cameraSpeed = SPEED * deltaTime; // adjust accordingly
	
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::ProcessMouseMovement(double xoffset, double yoffset){
	float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;
    if(Pitch > 89.0f)
        Pitch = 89.0f;
    if(Pitch < -89.0f)
        Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front); 
}

#endif