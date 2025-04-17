#version 330 core
layout (location = 0) in vec3 aPos; // 位置變數的屬性位置值為 0
layout (location = 1) in vec3 aColor; // 顏色變數的屬性位置值為 1

out vec3 ourColor; // 向片段著色器輸出一個顏色

void main(){
 gl_Position = vec4(aPos, 1.0);
 ourColor = aColor; // 將ourColor設定為我們從頂點資料得到的輸入顏色
}