#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "MyShader.h"

Shader ourShader("Shaders/shader.vs","Shaders/shader.fs");


void notused(){
    // OpenGL沒有自帶任何的矩陣和向量知識，所以我們必須定義自己的數學類別和函數。在教學中我們更希望抽象化所有的數學細節，使用已經做好了的數學庫。
    // 幸運的是，有個易於使用，專門為OpenGL量身訂做的數學庫，那就是GLM。
    // --------------------<GLM>--------------------
    // GLM是Open GL M athematics的縮寫，它是一個只有頭檔的函式庫，也就是說我們只要包含對應的頭檔就行了，不用連結和編譯。 
    // GLM可以在它們的網站上下載。把頭檔的根目錄複製到你的includes資料夾，然後你就可以使用這個函式庫了。
        // GLM函式庫從0.9.9版本起，預設會將矩陣類型初始化為一個零矩陣（所有元素均為0），而不是單位矩陣（對角元素為1，其它元素為0）。
        // 如果你使用的是0.9.9或0.9.9以上的版本，你需要將所有的矩陣初始化改為glm::mat4 mat = glm::mat4(1.0f)。
        // 如果你想與本教學的程式碼保持一致，請使用低於0.9.9版本的GLM，或改用上述程式碼初始化所有的矩陣。
    // 我們需要的GLM的大多數功能都可以從下面這3個頭檔中找到：
        // #include <glm/glm.hpp>
        // #include <glm/gtc/matrix_transform.hpp>
        // #include <glm/gtc/type_ptr.hpp>

    // 讓我們來看看是否可以利用我們剛學的變換知識把一個向量(1, 0, 0)位移(1, 1, 0)個單位（注意，我們把它定義為一個glm::vec4類型的值，齊次坐標設定為1.0）：
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // 譯註：下面就是矩陣初始化的一個例子，如果使用的是0.9.9以上版本，下面這行程式碼就需要改成 glm::mat4 trans = glm::mat4(1.0f) ，之後將不再進行提示
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;

    // 我們先用GLM內建的向量類別定義一個叫做vec的向量。接下來定義一個mat4類型的trans，預設是一個4×4單位矩陣。下一步是創建一個變換矩陣，
    // 我們是把單位矩陣和一個位移向量傳遞給glm::translate函數來完成這個工作的（然後用給定的矩陣乘以位移矩陣就能得到最後需要的矩陣）。 
    // 之後我們把向量乘以位移矩陣並且輸出最後的結果。如果你還記得位移矩陣是如何運作的話，得到的向量應該是(1 + 1, 0 + 1, 0 + 0)，
    // 也就是(2, 1, 0)。這個程式碼片段將會輸出210，所以這個位移矩陣是正確的。

    // 讓我們來做一些更有意思的事情，讓我們來旋轉和縮放之前教學中的那個箱子。首先我們把箱子逆時針旋轉90度。
    // 然後縮放0.5倍，使它變成原來的一半大。我們先來創建變換矩陣：
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    // 首先，我們把箱子在每個軸都縮放到0.5倍，然後沿著z軸旋轉90度。 GLM希望它的角度是弧度製的(Radian)，所以我們使用glm::radians將角度轉換為弧度。
    // 注意有紋理的那面矩形是在XY平面上的，所以我們需要把它繞著z軸旋轉。
    // 因為我們把這個矩陣傳遞給了GLM的每個函數，GLM會自動將矩陣相乘，而傳回的結果是一個包含了多個變換的變換矩陣。

    // 下一個大問題是：如何把矩陣傳給著色器？我們在前面簡單提到GLSL裡也有一個mat4類型。
    // 所以我們將修改頂點著色器讓其接收一個mat4的uniform變量，然後再用矩陣uniform乘以位置向量：
        // #version 330 core
        // layout (location = 0) in vec3 aPos;
        // layout (location = 1) in vec2 aTexCoord;

        // out vec2 TexCoord;

        // uniform mat4 transform;

        // void main()
        // {
        //     gl_Position = transform * vec4(aPos, 1.0f);
        //     TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
        // }

    //
        // GLSL也有mat2和mat3類型從而允許了像向量一樣的混合運算。
        // 前面提到的所有數學運算（像是標量-矩陣相乘，矩陣-向量相乘和矩陣-矩陣相乘）在矩陣類型裡都可以使用。當出現特殊的矩陣運算的時候我們會特別說明。
    // 在把位置向量傳給gl_Position之前，我們先加入一個uniform，並且將其與變換矩陣相乘。
    // 我們的箱子現在應該是原來的二分之一大小並且（向左）旋轉了90度。當然，我們仍需要把變換矩陣傳遞給著色器：
    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    // 我們先查詢uniform變數的位址，然後用有Matrix4fv字尾的glUniform函數把矩陣資料傳送給著色器。
    // 第一個參數你現在應該很熟悉了，它是uniform的位置值。第二個參數告訴OpenGL我們要傳送多少個矩陣，這裡是1。
    // 第三個參數詢問我們是否希望對我們的矩陣進行轉置(Transpose)，也就是說交換我們矩陣的行和列。 
    // OpenGL開發者通常使用一種內部矩陣佈局，叫做列主序(Column-major Ordering)佈局。 
    // GLM的預設佈局就是列主序，所以不需要轉置矩陣，我們填GL_FALSE。
    // 最後一個參數是真正的矩陣數據，但是GLM並不是把它們的矩陣儲存為OpenGL所希望接受的那種，因此我們要先用GLM的自帶的函數value_ptr來變換這些數據。

    // 我們創建了一個變換矩陣，在頂點著色器中聲明了一個uniform，並把矩陣發送給了著色器，著色器會變換我們的頂點座標。最後的結果應該看起來像這樣：
    // https://learnopengl-cn.github.io/img/01/07/transformations.png

    // 完美！我們的箱子向左側旋轉，並且是原來的一半大小，所以變換成功了。我們現在做些更有意思的，看看我們是否可以讓箱子隨著時間旋轉，
    // 我們也會重新把箱子放在視窗的右下角。要讓箱子隨著時間推移旋轉，我們必須在遊戲循環中更新變換矩陣，因為它在每個渲染迭代中都要更新。
    // 我們使用GLFW的時間函數來取得不同時間的角度：
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    // 要記住的是前面的例子中我們可以在任何地方聲明變換矩陣，但是現在我們必須在每個迭代中創建它，從而保證我們能夠不斷更新旋轉角度。
    // 這也意味著我們必須在每次遊戲循環的迭代中重新建立變換矩陣。通常在渲染場景的時候，我們也會有多個需要在每次渲染迭代中都用新值重新建立的變換矩陣
    // 這裡我們先把箱子圍繞原點(0, 0, 0)旋轉，之後，我們把旋轉過後的箱子位移到螢幕的右下角。
    // 記住，實際的變換順序應該與閱讀順序相反：儘管在程式碼中我們先位移再旋轉，實際的變換卻是先應用旋轉再是位移的。
    // 明白所有這些變換的組合，並且知道它們是如何應用在物體上是一件非常困難的事情。只有不斷地嘗試和實驗這些變換你才能快速地掌握它們。
    
    // 如果你做對了，你將看到下面的結果：
    // 這就是我們剛剛做到的！一個位移過的箱子，它會一直轉，一個變換矩陣就做到了！現在你可以明白為什麼矩陣在圖形領域是如此重要的工具了。
    // 我們可以定義無限數量的變換，而把它們組合為僅僅一個矩陣，如果願意的話我們還可以重複使用它。
    // 在著色器中使用矩陣可以省去重新定義頂點資料的功夫，它也能夠節省處理時間，因為我們沒有一直重新發送我們的資料（這是個非常慢的過程）。
    
}