#include "Headers/MyCamera.h"

Camera::Camera(const glm::vec3& cameraPos, const GLfloat& yaw, const GLfloat& pitch){
        this->pos   = cameraPos;
        this->yaw   = yaw;
        this->pitch = pitch;

        this->world_up          = My_Camera_Default::CAMERA_DEFAULT_WORLDUP;
        this->MovementSpeed     = My_Camera_Default::CAMERA_DEFAULT_SPEED;
        this->MouseSensitivity  = My_Camera_Default::CAMERA_DEFAULT_SENSITIVITY;
        this->Zoom              = My_Camera_Default::CAMERA_DEFAULT_ZOOM;

        Update_Camera_vectors();
}

void Camera::Update_Camera_vectors(){
    // no need normalize() because the lenght of front must be 1 (if we calculate like this)
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    right = glm::normalize(glm::cross(front, world_up));
}


glm::mat4 Camera::GetViewMatrix()const{
    return glm::lookAt(pos, pos+front, world_up);
    return glm::lookAt(pos, pos+front, glm::normalize(glm::cross(right, front)));
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        pos += front * velocity;
    if (direction == BACKWARD)
        pos -= front * velocity;
    if (direction == LEFT)
        pos -= right * velocity;
    if (direction == RIGHT)
        pos += right * velocity;
    if (direction == UP)
        pos.y += velocity;
    if (direction == DOWN)
        pos.y -= velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch){
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch){
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    
    Update_Camera_vectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset){
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f){
        Zoom = 1.0f;
    }else if (Zoom > 45.0f){
        Zoom = 45.0f;
    }
}

GLfloat Camera::getZoom()const{return Zoom;}
glm::vec3 Camera::getCameraPos()const{return pos;}