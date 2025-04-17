#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

struct Light {
    // vec3 position; // no longer necessary when using directional lights.
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec3 Light_direction;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Light light;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Light_direction = normalize(vec3(view * vec4(light.direction,0)));
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(view * model))) * aNormal);      // 但是對於一個高效的程式來說，你最好先在CPU上計算出法線矩陣，再透過uniform把它傳遞給著色器（就像模型矩陣一樣）。
    TexCoords = aTexCoords;
}

// Why vec3(view * vec4(lightPos, 1.0));    Why 1.0???
// 讓我們看看 LookAt矩陣
// 
//         ⎡ Rx Ry Rz 0 ⎤       ⎡ 1 0 0 -Px ⎤
// LookAt= ⎢ Ux Uy Uz 0 ⎥   *   ⎢ 0 1 0 -Py ⎥
//         ⎢ Dx Dy Dz 0 ⎥       ⎢ 0 0 1 -Pz ⎥
//         ⎣ 0  0  0  1 ⎦       ⎣ 0 0 0  1  ⎦
// 
// 我們可以發現若要逕行右邊的矩陣(平移) 若不是1.0的話 平移絕對會出問題