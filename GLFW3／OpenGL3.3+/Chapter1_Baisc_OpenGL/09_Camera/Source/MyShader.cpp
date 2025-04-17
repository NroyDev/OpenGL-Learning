#include "MyShader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath){
// 1. 從檔案中取得頂點/片段著色器的程式碼
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保證ifstream物件可以拋出例外：
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try{
    // 開啟檔案
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    // 讀取檔案的緩衝內容到資料流中
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // 關閉檔案
    vShaderFile.close();
    fShaderFile.close();
    // 轉換資料流到string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    }catch(std::ifstream::failure e){
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

// 2. 編譯著色器
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // 頂點著色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 輸出編譯的錯誤訊息（如果有的話）
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // 片段著色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // 輸出編譯的錯誤訊息（如果有的話）
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);      
    if (!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 著色器程式
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // 輸出link錯誤訊息（如果有的話）
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success){
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // link完後，我們不再需要這些了，還回去!
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

Shader::~Shader(){
    glDeleteProgram(ID);
}

void Shader::use(){ 
    glUseProgram(ID);
}
void Shader::setBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
} 
void Shader::setMat4(const std::string &name, const glm::mat4& mtx) const{ 
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mtx));
} 