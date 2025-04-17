#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Headers/MyShader.h"
#include "Headers/MyCamera.h"



void notused(){

    //---------------------------------------------<材質概論>---------------------------------------------
    // 在現實世界裡，每個物體都會對光產生不同的反應。例如，鋼製物體看起來通常會比陶土花瓶更閃閃發光，一個木頭箱子也不會與一個鋼製箱子反射同樣程度的光。
    // 有些物體反射光的時候不會有太多的散射(Scatter)，因而產生較小的高光點，而有些物體則會散射很多，產生一個有著更大半徑的高光點。
    // 如果我們想要在OpenGL中模擬多種類型的物體，我們必須針對每種表面定義不同的材質(Material)屬性。

    // 在上一節中，我們定義了一個物體和光的顏色，並結合環境光與鏡面強度分量，來決定物體的視覺輸出。
    // 當描述一個表面時，我們可以分別為三個光照分量定義一個材質顏色(Material Color)：
    // 環境光照(Ambient Lighting)、漫反射光照(Diffuse Lighting)和鏡面光照(Specular Lighting)。
    // 透過為每個分量指定一個顏色，我們就能夠對錶面的顏色輸出有細粒度的控制了。
    // 現在，我們再增加一個反光度(Shininess)分量，結合上述的三種顏色，我們就有了全部所需的材質屬性了：
        // #version 330 core
        // struct Material {
        //     vec3 ambient;
        //     vec3 diffuse;
        //     vec3 specular;
        //     float shininess;
        // }; 

        // uniform Material material;

    // 如你所見，我們為馮氏光照模型的每個分量定義一個顏色向量。ambient材質向量定義了在環境光照下這個表面反射的是什麼顏色，通常與表面的顏色相同。
    // diffuse材質向量定義了在漫反射光照下表面的顏色。漫反射顏色（和環境光照一樣）也被設定為我們期望的物體顏色。
    // specular材質向量設定的是表面上鏡面高光的顏色（甚至可能反映一個特定表面的顏色）。最後，shininess會影響鏡面高光的散射/半徑。

    // 有這4個元素定義一個物體的材質，我們能夠模擬許多現實世界中的材質。devernay.free.fr中的一個表格展示了一系列材質屬性，它們模擬了現實世界中的真實材質。
    // http://devernay.free.fr/cours/opengl/materials.html
    // 下圖展示了幾組現實世界的材質參數值對我們的立方體的影響：
    // https://learnopengl.com/img/lighting/materials_real_world.png
    // 可以看到，透過正確地指定一個物體的材質屬性，我們對這個物體的感知也就不同了。效果非常明顯，但是要獲得更真實的效果，我們需要以更複雜的形狀取代這個立方體。
    // 在模型載入章節中，我們會討論更複雜的形狀。
    
    // 要搞清楚一個物體正確的材質設定是個困難的工程，這主要需要實驗和豐富的經驗。用了不合適的材質而毀了物體的視覺品質是件常發生的事。
    // 讓我們試著在著色器中實現這樣的材質系統。


    //---------------------------------------------<設定材質>---------------------------------------------
    // 我們在片段著色器中創建了一個材質結構體的uniform，所以下面我們希望修改一下光照的計算來遵從新的材質屬性。
    // 由於所有材質變數都儲存在一個結構體中，我們可以從uniform變數material存取它們：
        // void main()
        // {
        // // 環境光
        // vec3 ambient = lightColor * material.ambient;

        // // 漫反射
        // vec3 norm = normalize(Normal);
        // vec3 lightDir = normalize(lightPos - FragPos);
        // float diff = max(dot(norm, lightDir), 0.0);
        // vec3 diffuse = lightColor * (diff * material.diffuse);

        // // 鏡面光
        // vec3 viewDir = normalize(viewPos - FragPos);
        // vec3 reflectDir = reflect(-lightDir, norm);
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // vec3 specular = lightColor * (spec * material.specular);

        // vec3 result = ambient + diffuse + specular;
        // FragColor = vec4(result, 1.0);
        // }
    // 可以看到，我們現在在需要的地方訪問了材質結構體中的所有屬性，並且這次是根據材質的顏色來計算最終的輸出顏色的。
    // 物體的每個材質屬性都乘以了它們各自對應的光照分量。
    // 我們現在可以透過設定適當的uniform來設定應用中物體的材質了。 GLSL中一個結構體在設定uniform時並無任何區別，結構體只是充當uniform變數們的一個命名空間。
    // 所以如果想填滿這個結構體的話，我們必須設定每個單獨的uniform，但要以結構體名為前綴：
        // lightingShader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // lightingShader.setFloat("material.shininess", 32.0f);
    // 我們將環境光和漫反射分量設定成我們想要讓物體所擁有的顏色，而將鏡面分量設定為一個中等亮度的顏色，我們不希望鏡面分量過於強烈。我們仍將反光度維持為32。
    // 現在我們能夠輕鬆地在應用中影響物體的材質了。運行程序，你會得到像這樣的結果：
    // https://learnopengl.com/img/lighting/materials_with_material.png
    // 不過看起來真的不太對勁？


    //---------------------------------------------<光的屬性>---------------------------------------------
    // 這個物體太亮了。物體過亮的原因是環境光、漫反射和鏡面光這三種顏色對任何一個光源都全力反射。光源對環境光、漫反射和鏡面光分量也分別有不同的強度。
    // 在前面的章節中，我們透過使用一個強度值來改變環境光和鏡面光強度的方式解決了這個問題。我們想做類似的事情，但這次是要為每個光照分量分別指定一個強度向量。
    // 如果我們假設lightColor是vec3(1.0)，程式碼會看起來像這樣：
        // vec3 ambient  = vec3(1.0) * material.ambient;
        // vec3 diffuse  = vec3(1.0) * (diff * material.diffuse);
        // vec3 specular = vec3(1.0) * (spec * material.specular);
    // 所以物體的每個材質屬性對每一個光照分量都回傳了最大的強度。對單一光源來說，這些vec3(1.0)數值同樣可以對每種光源分別改變，而這通常就是我們想要的。
    // 現在，物體的環境光分量完全地影響了立方體的顏色，可是環境光分量實際上不應該對最終的顏色有這麼大的影響，所以我們會將光源的環境光強度設定為一個小一點的值，從而限制環境光顏色：
        // vec3 ambient = vec3(0.1) * material.ambient;
    
    // 我們可以用同樣的方式影響光源的漫反射和鏡面光強度。這和我們在上一節中所做的極為相似，你可以認為我們已經創造了一些光照屬性來影響各個光照分量。
    // 我們希望為光照屬性創造類似材質結構體的東西：
        // struct Light {
        //     vec3 position;

        //     vec3 ambient;
        //     vec3 diffuse;
        //     vec3 specular;
        // };

        // uniform Light light;
    // 一個光源對它的ambient、diffuse和specular光照分量有著不同的強度。環境光照通常被設定為一個比較低的強度，因為我們不希望環境光顏色太過主導。
    // 光源的漫反射分量通常被設定為我們希望光所具有的那個顏色，通常是一個比較明亮的白色。鏡面光分量通常會保持為vec3(1.0)，以最大強度發光。注意我們也將光源的位置向量加入了結構體。

    // 和材質uniform一樣，我們需要更新片段著色器：
        // vec3 ambient  = light.ambient * material.ambient;
        // vec3 diffuse  = light.diffuse * (diff * material.diffuse);
        // vec3 specular = light.specular * (spec * material.specular);

    // 我們接下來在應用中設定光照強度：
        // lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // 將光線調暗了一些以搭配場景
        // lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // 現在我們已經調整了光照對物體材質的影響，我們得到了一個與上一節很相似的視覺效果。但這次我們有了對光線和物件材質的完全掌控：
    // 改變物體的視覺效果現在變得相對容易了。讓我們做點更有趣的事！


    //---------------------------------------------<不同的光源顏色>---------------------------------------------
    // 到目前為止，我們都只對光源設定了從白到灰到黑範圍內的顏色，這樣只會改變物體各個分量的強度，而不是它的真正顏色。由於現在能夠非常容易地存取光照的屬性了，
    // 我們可以隨著時間改變它們的顏色，從而獲得一些非常有趣的效果。由於所有的東西都在片段著色器中配置好了，修改光源的顏色非常簡單，並立刻創造一些很有趣的效果：
    // 你可以看到，不同的光照顏色能夠極大地影響物體的最終顏色輸出。由於光照顏色能夠直接影響物體能夠反射的顏色（回想顏色這一節），這對視覺輸出有顯著的影響。
    // 我們可以利用sin和glfwGetTime函數改變光源的環境光和漫反射顏色，因此很容易讓光源的顏色隨著時間而變化：
        // glm::vec3 lightColor;
        // lightColor.x = sin(glfwGetTime() * 2.0f);
        // lightColor.y = sin(glfwGetTime() * 0.7f);
        // lightColor.z = sin(glfwGetTime() * 1.3f);

        // glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // 降低影響
        // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影響

        // lightingShader.setVec3("light.ambient", ambientColor);
        // lightingShader.setVec3("light.diffuse", diffuseColor);


















}