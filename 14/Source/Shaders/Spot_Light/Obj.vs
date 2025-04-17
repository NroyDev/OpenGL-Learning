#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;
    // ambient、diffuse和specular光照分量強度
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;

    float constant;
    float linear;
    float quadratic;
};

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Light light;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3( model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);      // 但是對於一個高效的程式來說，你最好先在CPU上計算出法線矩陣，再透過uniform把它傳遞給著色器（就像模型矩陣一樣）。
    TexCoords = aTexCoords;
}
