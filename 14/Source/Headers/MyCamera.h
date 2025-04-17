#ifndef MYCAMERA_H
#define MYCAMERA_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// defalut camera settings
namespace My_Camera_Default{
    const glm::vec3 CAMERA_DEFAULT_POS(0.0f,0.0f,0.0f);
    const GLfloat CAMERA_DEFAULT_YAW         = 0.0f;
    const GLfloat CAMERA_DEFAULT_PITCH       = 0.0f;
    
    const glm::vec3 CAMERA_DEFAULT_WORLDUP   = glm::vec3(0.0f,1.0f,0.0f);
    const GLfloat CAMERA_DEFAULT_SPEED       = 5.0f;
    const GLfloat CAMERA_DEFAULT_SENSITIVITY = 0.1f;
    const GLfloat CAMERA_DEFAULT_ZOOM        = 45.0f;
}


class Camera{
    public:
        // constructor
        Camera(const glm::vec3& cameraPos = My_Camera_Default::CAMERA_DEFAULT_POS, const GLfloat& yaw = My_Camera_Default::CAMERA_DEFAULT_YAW, const GLfloat& pitch = My_Camera_Default::CAMERA_DEFAULT_YAW);
        
        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix()const;

        // 處按鍵輸入
        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
        void ProcessMouseScroll(GLfloat yoffset);

        GLfloat getZoom()const;
        glm::vec3 getCameraPos()const;
        glm::vec3 getCameraFront()const;

    private:
        // Camera baisc settings
        glm::vec3 pos;          //攝影機在世界中的位置
        glm::vec3 world_up;     //世界的正上方
        glm::vec3 front;
        glm::vec3 right;
        
        
        //Euler Angles
        GLfloat pitch;  //垂直方向的仰角
        GLfloat yaw;    //水平方向的偏航角

        // camera options
        GLfloat MovementSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;

        void Update_Camera_vectors();
};



#endif