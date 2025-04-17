#version 330 core

in vec3 LightingColor;
out vec4 FragColor;

uniform vec3 objectColor;

void main(){
    vec3 result = LightingColor * objectColor;
    FragColor = vec4(result, 1.0);
}