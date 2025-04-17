#version 330 core

struct Material {
    sampler2D diffuse;  // 在漫反射光照下表面的顏色，通常與表面的顏色相同。 環境光照的因為通常與diffuse相同，我們暫時移除他
    sampler2D specular; // 表面上鏡面高光的顏色（甚至可能反映一個特定表面的顏色）
    float     shininess;// 鏡面高光的散射/半徑。
}; 

struct Light {
    // vec3 position; // no longer necessary when using directional lights.
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Light_direction;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;


uniform Material material;
uniform Light light;
uniform vec3 lightColor;

void main(){
    // 環境光源
    vec3 ambient    = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // 漫反射
    vec3 lightDir   = normalize(-Light_direction);        // light direction
    float diff      = max(dot(Normal, lightDir), 0.0);      // 內積決定亮度
    vec3 diffuse    = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    

    // 鏡面光
    vec3 viewDir    = normalize(-FragPos);                  // 觀察者必在原點
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular   = light.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 result     = (ambient + diffuse + specular);
    FragColor       = vec4(result, 1.0);
}