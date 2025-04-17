#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 Position = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm     = normalize(Normal);              //normal vector
    vec3 lightDir = normalize(lightPos - Position);  //light direction
    float diff    = max(dot(norm, lightDir), 0.0);  //內積決定亮度
    vec3 diffuse  = diff * lightColor;

    float specularStrength = 1;
    float Shininess        = 32.0;                                          //反光度
    vec3 viewDir           = normalize(viewPos - Position);
    vec3 reflectDir        = reflect(-lightDir, norm);
    float spec             = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular          = specularStrength * spec * lightColor;

    LightingColor = ambient + diffuse + specular;
}