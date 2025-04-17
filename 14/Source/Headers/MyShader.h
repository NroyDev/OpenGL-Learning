#ifndef MYSHADER_H
#define MYSHADER_H

#include <glad/glad.h>; // 包含glad來取得所有的必須OpenGL頭檔

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader{
public:
    // 程式ID
    unsigned int ID;

    // 建構 讀取並建構著色器
    Shader(const char* vertexPath, const char* fragmentPath);
    // 解構
    ~Shader();
    // 使用/啟動程式
    void use();
    // uniform工具函數
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, const glm::vec3& vec) const;
    void setMat4(const std::string &name, const glm::mat4& mtx) const;
};
#endif