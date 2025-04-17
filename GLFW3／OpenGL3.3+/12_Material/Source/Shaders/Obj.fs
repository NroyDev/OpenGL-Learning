#version 330 core

struct Material {       // 參考: http://devernay.free.fr/cours/opengl/materials.html
    vec3 ambient;       // 環境光照下這個表面反射的是什麼顏色，通常與表面的顏色相同。
    vec3 diffuse;       // 在漫反射光照下表面的顏色，通常與表面的顏色相同。
    vec3 specular;      // 表面上鏡面高光的顏色（甚至可能反映一個特定表面的顏色）
    float shininess;    // 鏡面高光的散射/半徑。
}; 

struct Light {
    vec3 position;
    // ambient、diffuse和specular光照分量強度
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
out vec4 FragColor;


uniform Material material;
uniform Light light;
uniform vec3 lightColor;

void main(){
    // 環境光源
    vec3 ambient    = light.ambient * material.ambient;

    // 漫反射
    vec3 lightDir   = normalize(LightPos - FragPos);        // light direction
    float diff      = max(dot(Normal, lightDir), 0.0);      // 內積決定亮度
    vec3 diffuse    = (diff * material.diffuse) * light.diffuse;

    // 鏡面光
    vec3 viewDir    = normalize(-FragPos);                  // 觀察者必在原點
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular   = (spec * material.specular) * light.specular;

    vec3 result     = (ambient + diffuse + specular);
    FragColor       = vec4(result, 1.0);
}