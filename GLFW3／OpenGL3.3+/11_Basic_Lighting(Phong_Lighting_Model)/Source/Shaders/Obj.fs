#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main(){
    // 環境光源
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 lightDir = normalize(LightPos - FragPos);      // light direction
    float diff    = max(dot(Normal, lightDir), 0.0);    // 內積決定亮度
    vec3 diffuse  = diff * lightColor;

    // 鏡面光
    float specularStrength = 0.5;                       // 鏡面強度(Specular Intensity)
    float Shininess        = 32.0;                      // 反光度
    vec3 viewDir           = normalize(-FragPos);       // 觀察者必在原點
    vec3 reflectDir        = reflect(-lightDir, Normal);
    float spec             = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular          = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}