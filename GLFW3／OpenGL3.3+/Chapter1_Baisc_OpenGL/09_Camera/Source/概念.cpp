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
glm::vec3 direction(0,0,0);
float pitch,yaw,xpos,ypos,fov;
GLFWwindow* window;
bool firstMouse;
glm::mat4 projection = glm::mat4(1.0f);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void notused(){
    // 前面的教學中我們討論了觀察矩陣以及如何使用觀察矩陣移動場景（我們向後移動了一點）。 
    // OpenGL本身沒有攝影機(Camera)的概念，但我們可以透過把場景中的所有物體往相反方向移動的方式來模擬出攝影機，產生一種我們在移動的感覺，而不是場景在移動。
    // 本節我們將會討論如何在OpenGL中配置一個攝影機，並且將會討論FPS風格的攝影機，讓你能夠在3D場景中自由移動。
    // 我們也會討論鍵盤和滑鼠輸入，最終完成一個自訂的攝影機類別。

    // --------------------<攝影機/觀察空間>--------------------
    // 當我們討論攝影機/觀察空間(Camera/View Space)的時候，是在討論以攝影機的視角作為場景原點時場景中所有的頂點座標：
    // 觀察矩陣把所有的世界座標變換為相對於攝影機位置與方向的觀察座標。
    // 要定義一個攝影機，我們需要它在世界空間中的位置、觀察的方向、一個指向它右側的向量、一個指向它上方的向量。
    // 細心的讀者可能已經注意到我們實際上創建了一個三個單位軸相互垂直的、以攝影機的位置為原點的座標系。
    // https://learnopengl.com/img/getting-started/camera_axes.png

    // 1. 攝影機位置
    // 取得攝影機位置很簡單。攝影機位置簡單來說就是世界空間中一個指向攝影機位置的向量。我們把攝影機位置設定為上一節中的那個相同的位置：
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    // 不要忘記正z軸是從螢幕指向你的，如果我們希望攝影機向後移動，我們就沿著z軸的正方向移動。

    // 2. 攝影機方向
    // 下一個需要的向量是攝影機的方向，這裡指的是攝影機指向哪個方向。現在我們讓攝影機指向場景原點：(0, 0, 0)。
    // 還記得如果將兩個向量相減，我們就能得到這兩個向量的差嗎？用場景原點向量減去攝影機位置向量的結果就是攝影機的指向向量。
    // 由於我們知道攝影機指向z軸負方向，但我們希望方向向量(Direction Vector)指向攝影機的z軸正方向。
    // 如果我們交換相減的順序，我們就會得到一個指向攝影機正z軸方向的向量：
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        // 方向向量(Direction Vector)並不是最好的名字，因為它實際上指向從它到目標向量的相反方向
        // （譯註：注意看前面的那個圖，藍色的方向向量大概指向z軸的正方向，與攝影機實際指向的方向是正好相反的）。

    // 3. 右軸
    // 我們需要的另一個向量是一個右邊向量(Right Vector)，它代表攝影機空間的x軸的正方向。要取得右邊向量我們需要先使用一個小技巧：
    // 先定義一個上向量(Up Vector)。接下來把上向量和第二步得到的方向向量進行叉乘。兩個向量叉乘的結果會同時垂直於兩個向量，
    // 因此我們會得到指向x軸正方向的那個向量（如果我們交換兩個向量叉乘的順序就會得到相反的指向x軸負方向的向量） ：
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    // 4. 上軸
    // 現在我們已經有了x軸向量和z軸向量，取得一個指向攝影機的正y軸向量就相對簡單了：我們把右向量和方向向量進行叉乘：
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // 在叉乘和一些小技巧的幫助下，我們創建了所有構成觀察/攝影機空間的向量。對於想學到更多數學原理的讀者，提示一下，
    // 在線性代數中這個處理叫做格拉姆—施密特正交化(Gram-Schmidt Process)。使用這些攝影機向量我們就可以建立一個LookAt矩陣了，它在創建攝影機的時候非常有用。


    // --------------------<Look At>--------------------
    // 使用矩陣的優點之一是如果你使用3個相互垂直（或非線性）的軸定義了一個座標空間，你可以用這3個軸外加一個平移向量來建立一個矩陣，
    // 並且你可以用這個矩陣乘以任何向量來將其變換到那個座標空間。這正是LookAt矩陣所做的，現在我們有了3個相互垂直的軸和一個定義攝影機空間的位置座標，
    // 我們可以創建我們自己的LookAt矩陣了：
    // 
    //         ⎡ Rx Ry Rz 0 ⎤       ⎡ 1 0 0 -Px ⎤
    // LookAt= ⎢ Ux Uy Uz 0 ⎥   *   ⎢ 0 1 0 -Py ⎥
    //         ⎢ Dx Dy Dz 0 ⎥       ⎢ 0 0 1 -Pz ⎥
    //         ⎣ 0  0  0  1 ⎦       ⎣ 0 0 0  1  ⎦
    // 
    // 其中 R 是右邊向量，U 是上向量，D 是方向向量，P 是攝影機位置向量。
    // 注意，位置向量是相反的，因為我們最終希望把世界平移到與我們自身移動的相反方向。
    // 把這個LookAt矩陣當作觀察矩陣可以很有效率地把所有世界座標變換到剛剛定義的觀察空間。 LookAt矩陣就像它的名字表達的那樣：
    // 它會創造一個看著(Look at)給定目標的觀察矩陣。

    // 數學解釋
        // 這個矩陣目的是把觀察者位置作為原點，將物體的座標都減去攝影機的位置，使攝影機變成原點
        // ⎡ 1 0 0 -Px ⎤
        // ⎢ 0 1 0 -Py ⎥
        // ⎢ 0 0 1 -Pz ⎥
        // ⎣ 0 0 0  1  ⎦
        // 接下來要處理觀察者 x、y、z 實際的三個軸方向，假設這需要一個矩陣 M，因為是這個矩陣若遇到點 (Rx, Ry, Rz)，必須得到 (1, 0, 0) 的值，
        // 遇到點 (Ux, Uy, Uz)，必須得到 (0, 1, 0) 的值，遇到點 (Dx, Dy, Dz)，必須得到 (0, 0, 1) 的值           (要轉視角)
        //       ⎡ Rx Ry Rz 0 ⎤     ⎡ 1 0 0 0 ⎤
        // M  *  ⎢ Ux Uy Uz 0 ⎥  =  ⎢ 0 1 0 0 ⎥
        //       ⎢ Dx Dy Dz 0 ⎥     ⎢ 0 0 1 0 ⎥
        //       ⎣ 0  0  0  1 ⎦     ⎣ 0 0 0 1 ⎦
        // 而乘號右邊是個正交矩陣（Orthogonal matrix）正交矩陣的反矩陣就是轉置矩陣 //註釋: Orthogonal matrix: 每個column(或row)的向量內積為0且長度為1
        // 故 M 是
        // ⎡ Rx Ry Rz 0 ⎤
        // ⎢ Ux Uy Uz 0 ⎥
        // ⎢ Dx Dy Dz 0 ⎥
        // ⎣ 0  0  0  1 ⎦    
        // 將這兩個矩陣組合，即可得到Look at矩陣
    
    // 幸運的是，GLM已經提供了這些支援。我們要做的只是定義一個攝影機位置，一個目標位置和一個表示世界空間中的上向量的向量（我們計算右向量使用的那個上向量）。
    // 接著GLM就會創造一個LookAt矩陣，我們可以把它當作我們的觀察矩陣：
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::LookAt函數需要一個位置、目標和上向量。它會創建一個和在上一節使用的一樣的觀察矩陣。

    // 在討論使用者輸入之前，我們先來做一些有意思的事，把我們的攝影機在場景中旋轉。我們會將攝影機的注視點保持在(0, 0, 0)。
    // 我們需要用到一點三角函數的知識來在每一幀創建一個x和z座標，它會代表圓上的一點，我們將會使用它作為攝影機的位置。
    // 透過重新計算x和y座標，我們會遍歷圓上的所有點，這樣攝影機就會繞著場景旋轉了。
    // 我們預先定義這個圓的半徑radius，在每次渲染迭代中使用GLFW的glfwGetTime函數重新創建觀察矩陣，來擴充這個圓。
    float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); 


    // --------------------<Look At>--------------------
    // 讓攝影機繞著場景轉的確很有趣，但讓我們自己移動攝影機會更有趣！首先我們必須設定一個攝影機系統，所以在我們的程式前面定義一些攝影機變數很有用：
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    // LookAt函數現在成了：
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    // 我們首先將攝影機位置設定為先前定義的cameraPos。方向是目前的位置加上我們剛剛定義的方向向量。這樣能確保無論我們怎麼移動，攝影機都會注視著目標方向。
    // 讓我們擺弄一下這些向量，在按下某些按鈕時更新cameraPos向量。
    // 我們已經為GLFW的鍵盤輸入定義過一個processInput函數了，我們來新加入幾個需要檢查的按鍵指令：
    // void processInput(GLFWwindow *window){
    //     //...
    //     float cameraSpeed = 0.05f; // adjust accordingly
    //     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //         cameraPos += cameraSpeed * cameraFront;
    //     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //         cameraPos -= cameraSpeed * cameraFront;
    //     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // }
    // 當我們按下WASD鍵的任一個，攝影機的位置都會隨之更新。如果我們希望向前或向後移動，我們就把位置向量加上或減去方向向量。
    // 如果我們希望向左右移動，我們使用叉乘來建立一個右向量(Right Vector)，並沿著它相應移動就可以了。這樣就創造了使用攝影機時熟悉的橫移(Strafe)效果。

    // 注意，我們對右向量進行了標準化。如果我們沒有對這個向量進行標準化，最後的叉乘結果會根據cameraFront變數傳回大小不同的向量。如果我們不對向量進行標準化，
    // 我們就得根據攝影機的朝向不同加速或減速移動了，但如果進行了標準化移動就是勻速的。
    // 現在你就應該能夠移動攝影機了，雖然移動速度和系統有關，你可能需要調整一下cameraSpeed。


    // --------------------<移動速度>--------------------
    // 目前我們的移動速度是個常數。理論上沒什麼問題，但是實際情況下根據處理器的能力不同，有些人可能會比其他人每秒繪製更多幀，
    // 也就是以更高的頻率調用processInput函數。結果就是，根據配置的不同，有些人可能移動得很快，有些人會移動很慢。當你發布你的程式的時候，
    // 你必須確保它在所有硬體上移動速度都一樣。
    // 圖形程式和遊戲通常會追蹤一個時間差(Deltatime)變量，它儲存了渲染上一幀所花費的時間。我們把所有速度都去乘以deltaTime值。
    // 結果就是，如果我們的deltaTime很大，就代表上一幀的渲染花了更多時間，所以這一幀的速度需要變得更高來平衡渲染所花去的時間。
    // 使用這種方法時，無論你的電腦快或慢，攝影機的速度都會相應平衡，這樣每個用戶的體驗就都一樣了。
    // 我們追蹤兩個全域變數來計算deltaTime值：
    float deltaTime = 0.0f; // 目前影格(幀)與上一格的時間差
    float lastFrame = 0.0f; // 上一格(幀)的時間
    // 在每一幀中我們計算出新的deltaTime以備後用。
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // 現在我們有了deltaTime，在計算速度的時候可以考慮進去了：
    // void processInput(GLFWwindow *window)
    // {
    // float cameraSpeed = 2.5f * deltaTime;
    // ...
    // }
    // 與前面的部分結合在一起，我們有了一個更流暢點的攝影機系統：
    // 現在我們有了一個在任何系統上移動速度都一樣的攝影機。同樣，如果你卡住了，查看一下原始碼。我們可以看到任何移動都會影響傳回的deltaTime值。



    // --------------------<視角移動>--------------------
    // 只用鍵盤移動沒什麼意思。特別是我們還不能轉向，移動很受限。是時候加入滑鼠了！
    // 為了能夠改變視角，我們需要根據滑鼠的輸入改變cameraFront向量。然而，根據滑鼠移動改變方向向量有點複雜，需要一些三角函數。
    
    // 歐拉角
    // 歐拉角(Euler Angle)是可以表示3D空間中任何旋轉的3個值，由萊昂哈德·歐拉(Leonhard Euler)在18世紀提出。一共有3種歐拉角：
    // 俯仰角(Pitch)、偏航角(Yaw)和滾轉角(Roll)，下面的圖片展示了它們的意義：
    // https://learnopengl.com/img/getting-started/camera_pitch_yaw_roll.png
    // 俯仰角是描述我們如何往上或往下看的角，可以在第一張圖中看到。第二張圖展示了偏航角，偏航角表示我們往左和往右看的程度。滾轉角代表我們如何翻滾攝影機，
    // 通常在太空船的攝影機中使用。每個歐拉角都有一個值來表示，把三個角結合起來我們就能夠計算3D空間中任何的旋轉向量了。

    // 對於我們的攝影機系統來說，我們只關心俯仰角和偏航角，所以我們不會討論滾轉角。給定一個俯仰角和偏航角，我們可以把它們轉換成一個代表新的方向向量的3D向量。
    // 俯仰角和偏航角轉換為方向向量的處理需要一些三角學知識，我們先從最基本的情況開始：
    // 如果我們把斜邊邊長定義為1，我們就能知道鄰邊的長度是cos x / h =cos x / 1 =cos x
    // ，它的對邊是sin y/ h =sin y/ 1 =sin y
    // 。這樣我們得到了能夠得到x和y方向長度的一般公式，它們取決於所給的角度。我們使用它來計算方向向量的分量：
    // https://learnopengl.com/img/getting-started/camera_yaw.png
    // 這個三角形看起來和前面的三角形很像，所以如果我們想像自己在xz平面上，看向y軸，
    // 我們可以基於第一個三角形計算來計算它的長度/y方向的強度(Strength)（我們往上或往下看多少）。從圖中我們可以看到一個給定俯仰角的y值等於sin θ：
    direction.y = sin(glm::radians(pitch)); // 注意我們先把角度轉為弧度
    // 這裡我們只更新了y值，仔細觀察x和z分量也被影響了。從三角形中我們可以看到它們的值等於：
    direction.x = cos(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch));
    // 看看我們是否能夠為偏航角找到需要的分量：
    // 就像俯仰角的三角形一樣，我們可以看到x分量取決於cos(yaw)的值，z值同樣取決於偏航角的正弦值。把這個加到前面的值中，會得到基於俯仰角和偏航角的方向向量：
    direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw)); 
    // 翻譯：direction代表攝影機的前軸(Front)，這個前軸是和本文第一幅圖片的第二個攝影機的方向向量是相反的
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    // 這樣我們就有了一個可以把俯仰角和偏航角轉換成用來自由旋轉視角的攝影機的3維方向向量了。你可能會奇怪：我們怎麼得到俯仰角和偏航角？


    // --------------------<滑鼠輸入>--------------------
    // 偏航角和俯仰角是透過滑鼠（或手柄）移動獲得的，水平的移動影響偏航角，垂直的移動影響俯仰角。
    // 它的原理就是，儲存上一幀滑鼠的位置，在當前幀中我們目前計算滑鼠位置與上一幀的位置相差多少。
    // 如果水平/垂直差異越大那麼俯仰角或偏航角就改變越大，也就是攝影機需要移動更多的距離。

    // 首先我們要告訴GLFW，它應該隱藏遊標，並捕捉(Capture)它。捕捉遊標表示的是，
    // 如果焦點在你的程序上（譯註：即表示你正在操作這個程序，Windows中擁有焦點的程序標題欄通常是有顏色的那個，而失去焦點的程序標題欄則是灰色的），
    // 遊標應該停留在視窗中（除非程式失去焦點或退出）。我們可以用一個簡單地配置呼叫來完成：
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 在呼叫這個函數之後，無論我們怎麼去移動滑鼠，遊標都不會顯示了，它也不會離開視窗。對於FPS攝影機系統來說非常完美。

    // 為了計算俯仰角和偏航角，我們需要讓GLFW監聽滑鼠移動事件。 （和鍵盤輸入相似）我們會用一個回呼函數來完成，函數的原型如下：
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    // 這裡的xpos和ypos代表目前滑鼠的位置。當我們用GLFW註冊了回調函數之後，滑鼠一移動mouse_callback函數就會被呼叫：
    glfwSetCursorPosCallback(window, mouse_callback);

    // 在處理FPS風格攝影機的滑鼠輸入的時候，我們必須在最終獲取方向向量之前先做下面這幾步：
        // 1.計算滑鼠距上一格的偏移量。
        // 2.把偏移量加到攝影機的俯仰角和偏航角。
        // 3.對偏航角和俯仰角進行最大和最小值的限制。
        // 4.計算方向向量。
    
    // 第一步是計算滑鼠自上一格的偏移量。我們必須先在程式中儲存上一幀的滑鼠位置，我們把它的初始值設定為螢幕的中心（螢幕的尺寸是800x600）
        float lastX = 400, lastY = 300;
    // 然後在滑鼠的回呼函數中我們計算當前幀和上一幀滑鼠位置的偏移量：
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // 注意這裡是相反的，因為y座標是從底部往頂部依序增大的
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
    // 注意我們把偏移量乘以了sensitivity（靈敏度）值。如果我們忽略這個數值，滑鼠移動就會太大了；你可以自己實驗一下，找到適合自己的靈敏度值。

    // 接下來我們把偏移加到全域變數pitch和yaw：
        yaw   += xoffset;
        pitch += yoffset;

    // 第三步，我們需要為攝影機增加一些限制，這樣攝影機就不會發生奇怪的移動了（這樣也會避免一些奇怪的問題）。
    // 對於俯仰角，要讓使用者不能看向高於89度的地方（在90度時視角會發生逆轉，所以我們把89度當作極限），同樣也不允許小於-89度。
    // 這樣能夠保證使用者只能看到天空或腳下，但是不能超越這個限制。我們可以在值超過限制的時候將其改為極限值來實現：
        if(pitch > 89.0f)
            pitch =  89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
    // 注意我們沒有給偏航角設定限制，這是因為我們不希望限制使用者的水平旋轉。當然，給偏航角設定限制也很容易，如果你願意可以自己實現。
    
    // 第四也是最後一步，就是透過俯仰角和偏航角來計算以獲得真正的方向向量：
        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        cameraFront = glm::normalize(front);

    // 計算出來的方向向量就會包含根據滑鼠移動計算出來的所有旋轉了。由於cameraFront向量已經包含在GLM的lookAt函數中，我們這就沒什麼問題了。
    // 如果你現在運行程式碼，你會發現在視窗第一次取得焦點的時候攝影機會突然跳一下。這個問題產生的原因是，在你的滑鼠移動進視窗的那一刻，
    // 滑鼠回呼函數就會被調用，這時候的xpos和ypos會等於滑鼠剛剛進入螢幕的那個位置。這通常是一個距離螢幕中心很遠的地方，
    // 因而產生一個很大的偏移量，所以就會跳了。我們可以簡單的使用一個bool變數來檢驗我們是否是第一次取得滑鼠輸入，
    // 如果是，那麼我們先把滑鼠的初始位置更新為xpos和ypos值，這樣就能解決這個問題；接下來的滑鼠移動就會使用剛進入的滑鼠位置座標來計算偏移量了：
    if(firstMouse) // 這個bool變數初始時是設定為true的
    {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
    }

    // 最後的程式碼應該是這樣的：
    // void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    // {
    //     if(firstMouse)
    //     {
    //         lastX = xpos;
    //         lastY = ypos;
    //         firstMouse = false;
    //     }

    //     float xoffset = xpos - lastX;
    //     float yoffset = lastY - ypos; 
    //     lastX = xpos;
    //     lastY = ypos;

    //     float sensitivity = 0.05;
    //     xoffset *= sensitivity;
    //     yoffset *= sensitivity;

    //     yaw   += xoffset;
    //     pitch += yoffset;

    //     if(pitch > 89.0f)
    //         pitch = 89.0f;
    //     if(pitch < -89.0f)
    //         pitch = -89.0f;

    //     glm::vec3 front;
    //     front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    //     front.y = sin(glm::radians(pitch));
    //     front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //     cameraFront = glm::normalize(front);
    // }


    // --------------------<縮放>--------------------
    // 作為我們攝影機系統的一個附加內容，我們也會來實作一個縮放(Zoom)介面。在之前的教學中我們說視野(Field of View)或fov定義了我們可以看到場景中多大的範圍。
    // 當視野變小時，場景投影出來的空間就會減少，產生放大(Zoom In)了的感覺。我們會使用滑鼠的滾輪來放大。
    // 就像滑鼠移動、鍵盤輸入一樣，我們需要一個滑鼠滾輪的回呼函數：
    // void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    //     if(fov >= 1.0f && fov <= 45.0f)
    //         fov -= yoffset;
    //     if(fov <= 1.0f)
    //         fov = 1.0f;
    //     if(fov >= 45.0f)
    //         fov = 45.0f;
    // }
    // 當滾動滑鼠滾輪的時候，yoffset值代表我們垂直滾動的大小。當scroll_callback函數被呼叫後，我們改變全域變數fov變數的內容。
    // 因為45.0f是預設的視野值，我們將會把縮放等級(Zoom Level)限制在1.0f到45.0f。
    // 我們現在在每一幀都必須把透視投影矩陣上傳到GPU，但現在使用fov變數作為它的視野：
    projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
    // 最後不要忘記註冊滑鼠滾輪的回呼函數：
    glfwSetScrollCallback(window, scroll_callback);
    
    // 現在，我們就實現了一個簡單的攝影機系統了，它能夠讓我們在3D環境中自由移動。
        // 注意，使用歐拉角的攝影機系統並不完美。根據你的視角限製或配置，你仍然可能引入萬向節死鎖問題。
        // 最好的攝影機系統是使用四元數(Quaternions)的，但我們將會把這個留到後面討論。 （譯註：這裡可以查看四元數攝影機的實現）


    // --------------------<攝影機 class>--------------------
    // 接下來的教學中，我們將會一直使用一個攝影機來瀏覽場景，從各個角度觀察結果。然而，由於一個攝影機會佔用每篇教程很大的篇幅，
    // 我們將從細節中抽像出來，創建我們自己的攝影機對象，它會完成大多數的工作，而且還會提供一些附加的功能。與著色器教程不同，
    // 我們不會帶你一步一步創建攝影機類，我們只會提供你一份（有完整註釋的）代碼，如果你想知道它的內部構造的話可以自己去閱讀。

    // 和著色器物件一樣，我們把攝影機類別寫在一個單獨的頭檔中。你可以在這裡找到它，你現在應該能夠理解所有的程式碼了。我們建議您至少看一看這個類，
    // 看看如何創建一個自己的攝影機類。

    // 我們介紹的攝影機系統是一個FPS風格的攝影機，它能夠滿足大多數情況需要，而且與歐拉角兼容，但是在創建不同的攝影機系統，例如飛行模擬攝影機，
    // 時就要當心。每個攝影機系統都有自己的優點和不足，所以確保對它們進行了詳細研究。例如，這台FPS攝影機不允許俯仰角大於90度，
    // 而且我們使用了固定的上向量(0, 1, 0)，這在需要考慮滾轉角的時候就不能用了。

    // 使用新攝影機對象，更新後版本的源碼可以在這裡找到。




}