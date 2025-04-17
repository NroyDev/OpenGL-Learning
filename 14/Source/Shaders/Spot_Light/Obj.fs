#version 330 core

struct Material {
    sampler2D diffuse;  // 在漫反射光照下表面的顏色，通常與表面的顏色相同。 環境光照的因為通常與diffuse相同，我們暫時移除他
    sampler2D specular; // 表面上鏡面高光的顏色（甚至可能反映一個特定表面的顏色）
    float     shininess;// 鏡面高光的散射/半徑。
}; 

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

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;


uniform Material material;
uniform Light light;
uniform vec3 lightColor;

void main(){
    // 手電筒效果
    vec3 lightDir   = normalize(light.position - FragPos);        // light direction
    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.cutOff){   // 執行光照計算
        // 環境光源
        vec3 ambient    = light.ambient * vec3(texture(material.diffuse, TexCoords));

        // 漫反射
        // vec3 lightDir   = normalize(LightPos - FragPos);        // light direction
        float diff      = max(dot(Normal, lightDir), 0.0);      // 內積決定亮度
        vec3 diffuse    = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        // 鏡面光
        vec3 viewDir    = normalize(-FragPos);                  // 觀察者必在原點
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular   = light.specular * spec * vec3(texture(material.specular, TexCoords));

        // 衰減
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient  *= attenuation; 
        diffuse  *= attenuation;
        specular *= attenuation;
        vec3 result     = (ambient + diffuse + specular);
        FragColor       = vec4(result, 1.0);
    }else{                      // 否則，使用環境光，讓場景在聚光之外時不至於完全黑暗
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
    }

}